#include "gdscript_language_server.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor/editor_node.h"

Error GDScriptLanguageServer::parse_script_file(const String &p_path) {

	FileAccessRef f = FileAccess::open(p_path, FileAccess::READ);
	if (f && f->is_open()) {

		GDScriptParser* parser = NULL;
		if (!script_cache.has(p_path)) {
			parser = memnew(GDScriptParser);
			script_cache.insert(p_path, parser);
		} else {
			parser = script_cache[p_path];
		}

		Error err = parser->parse(f->get_as_utf8_string(), p_path, true);
		if (OK == err) {
//			this->current_script_node_tree = parser->get_parse_tree();
		}
		return err;
	}
	return ERR_FILE_CANT_OPEN;
}

GDScriptLanguageServer::GDScriptLanguageServer() {
	thread = NULL;
	thread_exit = false;
	_threaded = false;
	server.instance();
	_EDITOR_DEF("network/language_server/remote_port", 6008);
	_EDITOR_DEF("network/language_server/use_thread", _threaded);
}

void GDScriptLanguageServer::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			start();
			break;
		case NOTIFICATION_EXIT_TREE:
			stop();
			break;
		case NOTIFICATION_PROCESS: {
			if (!_threaded) {
				_poll();
			}
		} break;
	}
}

void GDScriptLanguageServer::_poll() {
	GLOBAL_LOCK_FUNCTION

	if (server->is_connection_available()) {
		connection = server->take_connection();
	}

	if (connection.is_valid() && connection->get_status() == StreamPeerTCP::STATUS_CONNECTED) {
		recived_message_queue.clear();
		while (connection->get_available_bytes()) {
			recived_message_queue.push_back(connection->get_utf8_string());
		}

		_iteration();

		for (int i=0; i < sending_message_queue.size(); ++i) {
			connection->put_utf8_string(sending_message_queue[i]);
		}
		sending_message_queue.clear();
	}
}

void GDScriptLanguageServer::_iteration() {
	for (int i=0; i < recived_message_queue.size(); i++) {
		String message = recived_message_queue[i];
		String ret = jsonrpc.process_string(message);
		if (ret.length()) {
			sending_message_queue.push_back(ret);
		}
	}
}

void GDScriptLanguageServer::thread_main(void *p_userdata) {
	GDScriptLanguageServer * self =  static_cast<GDScriptLanguageServer*>(p_userdata);
	while(!self->thread_exit) {
		self->_poll();
		OS::get_singleton()->delay_usec(10);
	}
}

void GDScriptLanguageServer::start() {
	int port = (int)_EDITOR_GET("network/language_server/remote_port");
	if (server->listen(port) == OK) {
		EditorNode::get_log()->add_message("** GDScript Language Server Started **");
		_threaded = (bool)_EDITOR_GET("network/language_server/use_thread");
		if (_threaded) {
			ERR_FAIL_COND(thread != NULL || thread_exit);
			thread_exit = false;
			thread = Thread::create(GDScriptLanguageServer::thread_main, this);
		}
		set_process(!_threaded);
	}
}

void GDScriptLanguageServer::stop() {
	if (_threaded) {
		ERR_FAIL_COND(NULL == thread || !thread_exit);
		thread_exit = true;
		Thread::wait_to_finish(thread);
	}
	server->stop();
	EditorNode::get_log()->add_message("** GDScript Language Server Stopped **");
}
