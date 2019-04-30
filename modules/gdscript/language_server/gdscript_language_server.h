#ifndef GDSCRIPT_LANGUAGE_SERVER_H
#define GDSCRIPT_LANGUAGE_SERVER_H


#include "../gdscript_parser.h"
#include "editor/editor_plugin.h"
#include "core/io/tcp_server.h"
#include "modules/jsonrpc/jsonrpc.h"

class GDScriptLanguageServer : public EditorPlugin {
	GDCLASS(GDScriptLanguageServer, EditorPlugin);

	Map<String, GDScriptParser*> script_cache;

	Vector<String> sending_message_queue;
	Vector<String> recived_message_queue;

	Ref<TCP_Server> server;
	Ref<StreamPeerTCP> connection;

	Thread *thread;
	static void thread_main(void *p_userdata);
	bool thread_exit;
	bool _threaded;

	JSONRPC jsonrpc;

private:
	void _notification(int p_what);
	void _poll();
	void _iteration();
public:
	Error parse_script_file(const String& p_path);


	GDScriptLanguageServer();

	void start();
	void stop();

};

#endif // GDSCRIPT_LANGUAGE_SERVER_H
