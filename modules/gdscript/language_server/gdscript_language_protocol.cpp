#include "gdscript_language_protocol.h"
#include "core/io/json.h"
#include "core/os/copymem.h"
#include "core/project_settings.h"

GDScriptLanguageProtocol *GDScriptLanguageProtocol::singleton = NULL;

void GDScriptLanguageProtocol::on_data_received(int id) {
	lastest_client_id = id;
	Ref<WebSocketPeer> peer = server->get_peer(id);
	PoolByteArray data;
	if (OK == peer->get_packet_buffer(data)) {
		String message;
		message.parse_utf8((const char *)data.read().ptr());
		if (message.begins_with("Content-Length:")) return;
		print_line(message);
		String output = process_message(message);
		if (!output.empty()) {
			CharString charstr = output.utf8();
			peer->put_packet((const uint8_t *)charstr.ptr(), charstr.length());
		}
	}
}

void GDScriptLanguageProtocol::on_client_connected(int id, const String &protocal) {
	clients.set(id, server->get_peer(id));
}

void GDScriptLanguageProtocol::on_client_disconnected(int id, bool was_clean_close) {
	clients.erase(id);
}

String GDScriptLanguageProtocol::process_message(const String &text) {
	String ret = process_string(text);
	if (ret.empty()) {
		return ret;
	} else {
		return format_output(ret);
	}
}

String GDScriptLanguageProtocol::format_output(const String &text) {

	String header = "Content-Length: ";
	CharString charstr = text.utf8();
	size_t len = charstr.length();
	header += itos(len);
	header += "\r\n\r\n";

	return header + text;
}

void GDScriptLanguageProtocol::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "params"), &GDScriptLanguageProtocol::initialize);
	ClassDB::bind_method(D_METHOD("initialized", "params"), &GDScriptLanguageProtocol::initialized);
	ClassDB::bind_method(D_METHOD("on_data_received"), &GDScriptLanguageProtocol::on_data_received);
	ClassDB::bind_method(D_METHOD("on_client_connected"), &GDScriptLanguageProtocol::on_client_connected);
	ClassDB::bind_method(D_METHOD("on_client_disconnected"), &GDScriptLanguageProtocol::on_client_disconnected);
}

Dictionary GDScriptLanguageProtocol::initialize(const Dictionary &params) {

	lsp::InitializeResult ret;

	return ret.to_json();
}

void GDScriptLanguageProtocol::initialized(const Variant &p_params) {

	Dictionary params;
	params["type"] = 3;
	params["message"] = "GDScript Language Server initialized!";
	Dictionary test_message = make_notification("window/showMessage", params);

	if (Ref<WebSocketPeer> *peer = clients.getptr(lastest_client_id)) {
		String msg = JSON::print(test_message);
		msg = format_output(msg);
		CharString charstr = msg.utf8();
		(*peer)->put_packet((const uint8_t *)charstr.ptr(), charstr.length());
	}
}

void GDScriptLanguageProtocol::poll() {
	server->poll();
}

Error GDScriptLanguageProtocol::start(int port) {
	if (server == NULL) {
		server = dynamic_cast<WebSocketServer *>(ClassDB::instance("WebSocketServer"));
		server->connect("data_received", this, "on_data_received");
		server->connect("client_connected", this, "on_client_connected");
		server->connect("client_disconnected", this, "on_client_disconnected");
	}
	return server->listen(port);
}

void GDScriptLanguageProtocol::stop() {
	server->stop();
}

void GDScriptLanguageProtocol::notify_all_clients(const String &p_method, const Variant &p_params) {

	Dictionary message = make_notification(p_method, p_params);
	String msg = JSON::print(message);
	msg = format_output(msg);
	CharString charstr = msg.utf8();
	const int *p_id = NULL;
	while ((p_id = clients.next(p_id))) {
		Ref<WebSocketPeer> peer = clients.get(*p_id);
		(*peer)->put_packet((const uint8_t *)charstr.ptr(), charstr.length());
	}
}

void GDScriptLanguageProtocol::notify_client(const String &p_method, const Variant &p_params, int p_client) {

	if (p_client == -1) {
		p_client = lastest_client_id;
	}

	Ref<WebSocketPeer> *peer = clients.getptr(p_client);
	ERR_FAIL_COND(peer == NULL);

	Dictionary message = make_notification(p_method, p_params);
	String msg = JSON::print(message);
	msg = format_output(msg);
	CharString charstr = msg.utf8();

	(*peer)->put_packet((const uint8_t *)charstr.ptr(), charstr.length());
}

GDScriptLanguageProtocol::GDScriptLanguageProtocol() {
	server = NULL;
	singleton = this;
	set_scope("textDocument", &text_document);
	set_scope("workspace", &workspace);
	workspace.root = ProjectSettings::get_singleton()->get_resource_path();
}

GDScriptLanguageProtocol::~GDScriptLanguageProtocol() {
	memdelete(server);
	server = NULL;
}
