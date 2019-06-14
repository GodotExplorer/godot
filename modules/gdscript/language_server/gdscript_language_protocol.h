#ifndef GDSCRIPT_PROTOCAL_SERVER_H
#define GDSCRIPT_PROTOCAL_SERVER_H

#include "gdscript_text_document.h"
#include "gdscript_workspace.h"
#include "lsp.hpp"
#include "modules/jsonrpc/jsonrpc.h"
#include "modules/websocket/websocket_peer.h"
#include "modules/websocket/websocket_server.h"

class GDScriptLanguageProtocol : public JSONRPC {
	GDCLASS(GDScriptLanguageProtocol, JSONRPC)

	enum LSPErrorCode {
		RequestCancelled = -32800,
		ContentModified = -32801,
	};

	static GDScriptLanguageProtocol *singleton;

	HashMap<int, Ref<WebSocketPeer> > clients;
	WebSocketServer *server;
	int lastest_client_id;

	GDScriptTextDocument text_document;
	GDScriptWorkspace workspace;

	void on_data_received(int id);
	void on_client_connected(int id, const String &protocal);
	void on_client_disconnected(int id, bool was_clean_close);

	String process_message(const String &text);
	String format_output(const String &text);

protected:
	static void _bind_methods();

	Dictionary initialize(const Dictionary &params);
	void initialized(const Variant &p_params);

public:
	_FORCE_INLINE_ static GDScriptLanguageProtocol *get_singleton() { return singleton; }
	_FORCE_INLINE_ GDScriptWorkspace &get_workspace() { return workspace; }

	void poll();
	Error start(int port);
	void stop();

	void notify_all_clients(const String &p_method, const Variant &p_params = Variant());
	void notify_client(const String &p_method, const Variant &p_params = Variant(), int p_client = -1);

	GDScriptLanguageProtocol();
	~GDScriptLanguageProtocol();
};

#endif
