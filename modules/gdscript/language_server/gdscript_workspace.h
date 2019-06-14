#ifndef GDSCRIPT_WORKSPACE_H
#define GDSCRIPT_WORKSPACE_H

#include "../gdscript.h"
#include "../gdscript_parser.h"
#include "core/variant.h"
#include "editor/editor_node.h"
#include "gdscript_extend_parser.h"
#include "lsp.hpp"

class GDScriptWorkspace : public Reference {
	GDCLASS(GDScriptWorkspace, Reference);

protected:
	static void _bind_methods();
	void remove_cache_parser(const String &p_path);

private:
	String last_content;
	String last_file_path;

private:
	String add_cursor_to_script(String &p_content, const int &p_cur_line, const int &p_cur_char);
	String convert_to_relative_path(String p_path);

public:
	String root;
	Map<String, ExtendGDScriptParser *> scripts;
	Map<String, ExtendGDScriptParser *> parse_results;

public:
	Array symbol(const Dictionary &p_params);

public:
	Error parse_script(const String &p_path, const String &p_content);
	Vector<ScriptCodeCompletionOption> fetch_completion(const int &p_cur_line, const int &p_cur_char);

	void publish_diagnostics(const String &p_path);

	~GDScriptWorkspace();
};

#endif
