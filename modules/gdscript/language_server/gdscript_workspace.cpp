#include "gdscript_workspace.h"
#include "../gdscript.h"
#include "../gdscript_parser.h"
#include "gdscript_language_protocol.h"

void GDScriptWorkspace::_bind_methods() {
	ClassDB::bind_method(D_METHOD("symbol"), &GDScriptWorkspace::symbol);
}

void GDScriptWorkspace::remove_cache_parser(const String &p_path) {
	Map<String, ExtendGDScriptParser *>::Element *parser = parse_results.find(p_path);
	Map<String, ExtendGDScriptParser *>::Element *script = scripts.find(p_path);
	if (parser && script) {
		if (script->get() && script->get() == script->get()) {
			memdelete(script->get());
		} else {
			memdelete(script->get());
			memdelete(parser->get());
		}
		parse_results.erase(p_path);
		scripts.erase(p_path);
	} else if (parser) {
		memdelete(parser->get());
		parse_results.erase(p_path);
	} else if (script) {
		memdelete(script->get());
		scripts.erase(p_path);
	}
}

Array GDScriptWorkspace::symbol(const Dictionary &p_params) {
	String query = p_params["query"];
	Array arr;
	if (!query.empty()) {
		for (Map<String, ExtendGDScriptParser *>::Element *E = scripts.front(); E; E = E->next()) {
			Vector<lsp::SymbolInformation> script_symbols;
			E->get()->get_symbols().symbol_tree_as_list(E->key(), script_symbols);
			for (int i = 0; i < script_symbols.size(); ++i) {
				if (query.is_subsequence_ofi(script_symbols[i].name)) {
					arr.push_back(script_symbols[i].to_json());
				}
			}
		}
	}
	return arr;
}

static Node *_find_node_for_script(Node *p_base, Node *p_current, const Ref<Script> &p_script) {

	if (p_current->get_owner() != p_base && p_base != p_current)
		return NULL;
	Ref<Script> c = p_current->get_script();
	if (c == p_script)
		return p_current;
	for (int i = 0; i < p_current->get_child_count(); i++) {
		Node *found = _find_node_for_script(p_base, p_current->get_child(i), p_script);
		if (found)
			return found;
	}

	return NULL;
}

Error GDScriptWorkspace::parse_script(const String &p_path, const String &p_content) {
	ExtendGDScriptParser *parser = memnew(ExtendGDScriptParser);
	Error err = parser->parse(p_content, p_path);

	last_file_path = p_path;
	last_content = p_content;

	Map<String, ExtendGDScriptParser *>::Element *last_parser = parse_results.find(p_path);
	Map<String, ExtendGDScriptParser *>::Element *last_script = scripts.find(p_path);

	if (err == OK) {
		remove_cache_parser(p_path);
		parse_results[p_path] = parser;
		scripts[p_path] = parser;
	} else {
		if (last_parser && last_script && last_parser->get() != last_script->get()) {
			memdelete(last_parser->get());
		}
		parse_results[p_path] = parser;
	}

	publish_diagnostics(p_path);

	return err;
}

String GDScriptWorkspace::add_cursor_to_script(String &p_content, const int &p_cur_line, const int &p_cur_char) {
	int index_counter = 0;
	int line_counter = 0;
	int line_beginning_counter = 0;
	int char_counter = 0;
	CharType cursor = 0xFFFF;

	while (index_counter < p_content.size()) {
		index_counter++;

		if (p_content[index_counter] == '\n') {
			line_counter++;
			char_counter = 0;
			line_beginning_counter = index_counter;
		} else {
			if (p_content[index_counter] == ' ') {
				line_beginning_counter = index_counter + 1;
			}
			char_counter++;
		}

		if (line_counter == p_cur_line && char_counter == p_cur_char) {
			p_content = p_content.insert(index_counter + 1, String(&cursor));
			break;
		}
	}

	return p_content.substr(line_beginning_counter, index_counter - line_beginning_counter + 1);
}

String GDScriptWorkspace::convert_to_relative_path(String p_path) {
	String local_path = (p_path.substr(7, p_path.size() - 7)); // removes 'file://' from it
	String res_path = ProjectSettings::get_singleton()->get_resource_path();
	int index_counter = 0;
	while (res_path[index_counter] == local_path[index_counter]) {
		index_counter++;
	}

	local_path = "res:/" + local_path.substr(index_counter, local_path.size() - index_counter);
	return local_path;
}

Vector<ScriptCodeCompletionOption> GDScriptWorkspace::fetch_completion(const int &p_cur_line, const int &p_cur_char) {
	Node *base = EditorNode::get_singleton()->get_tree()->get_edited_scene_root();

	String p_content = last_content;
	String relative_path = convert_to_relative_path(last_file_path);

	RES script = ResourceLoader::load(relative_path);
	if (base) {
		base = _find_node_for_script(base, base, script);
	}

	String intellisense_word = add_cursor_to_script(p_content, p_cur_line, p_cur_char);

	bool forced = false;
	List<ScriptCodeCompletionOption> completion_strings;
	String hint;
	GDScriptLanguage::get_singleton()->complete_code(p_content, script->get_path().get_base_dir(), base, &completion_strings, forced, hint);

	print_line(">> " + intellisense_word);

	Vector<ScriptCodeCompletionOption> completion_options_casei;
	Vector<ScriptCodeCompletionOption> completion_options;

	for (int i = 0; i < completion_strings.size(); i++) {
		if (completion_strings[i].display.find(intellisense_word) != -1) {
			completion_options.push_back(completion_strings[i]);
		} else if (completion_strings[i].display.to_lower().find(intellisense_word.to_lower()) != -1) {
			completion_options_casei.push_back(completion_strings[i]);
		}
	}

	completion_options.append_array(completion_options_casei);

	if (completion_options.size() == 0) {
		for (int i = 0; i < completion_strings.size(); i++) {
			if (intellisense_word.is_subsequence_of(completion_strings[i].display)) {
				completion_options.push_back(completion_strings[i]);
			}
		}
	}

	if (completion_options.size() == 0) {
		for (int i = 0; i < completion_strings.size(); i++) {
			if (intellisense_word.is_subsequence_ofi(completion_strings[i].display)) {
				completion_options.push_back(completion_strings[i]);
			}
		}
	}

	return completion_options;
}

void GDScriptWorkspace::publish_diagnostics(const String &p_path) {
	Dictionary params;
	Array errors;
	const Map<String, ExtendGDScriptParser *>::Element *ele = parse_results.find(p_path);
	if (ele) {
		const Vector<lsp::Diagnostic> &list = ele->get()->get_diagnostics();
		errors.resize(list.size());
		for (int i = 0; i < list.size(); ++i) {
			errors[i] = list[i].to_json();
		}
	}
	params["diagnostics"] = errors;
	params["uri"] = p_path;
	GDScriptLanguageProtocol::get_singleton()->notify_client("textDocument/publishDiagnostics", params);
}

GDScriptWorkspace::~GDScriptWorkspace() {
	Set<String> cached_parsers;
	for (Map<String, ExtendGDScriptParser *>::Element *E = parse_results.front(); E; E = E->next()) {
		cached_parsers.insert(E->key());
	}
	for (Map<String, ExtendGDScriptParser *>::Element *E = scripts.front(); E; E = E->next()) {
		cached_parsers.insert(E->key());
	}
	for (Set<String>::Element *E = cached_parsers.front(); E; E = E->next()) {
		remove_cache_parser(E->get());
	}
}
