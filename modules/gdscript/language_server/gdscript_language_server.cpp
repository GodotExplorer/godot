#include "gdscript_language_server.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor/editor_node.h"

GDScriptLanguageServer::GDScriptLanguageServer() {
	thread = NULL;
	thread_exit = false;
	_threaded = false;
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
				protocol.poll();
			}
		} break;
	}
}

void GDScriptLanguageServer::thread_main(void *p_userdata) {
	GDScriptLanguageServer *self = static_cast<GDScriptLanguageServer *>(p_userdata);
	while (!self->thread_exit) {
		self->protocol.poll();
		OS::get_singleton()->delay_usec(10);
	}
}

void GDScriptLanguageServer::start() {
	int port = (int)_EDITOR_GET("network/language_server/remote_port");
	if (protocol.start(port) == OK) {
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
	protocol.stop();
	EditorNode::get_log()->add_message("** GDScript Language Server Stopped **");
}

void register_lsp_types() {
	ClassDB::register_class<GDScriptLanguageProtocol>();
	ClassDB::register_class<GDScriptTextDocument>();
	ClassDB::register_class<GDScriptWorkspace>();
}
