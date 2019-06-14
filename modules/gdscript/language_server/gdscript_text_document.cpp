#include "gdscript_text_document.h"
#include "gdscript_language_protocol.h"

void GDScriptTextDocument::_bind_methods() {
	ClassDB::bind_method(D_METHOD("didOpen"), &GDScriptTextDocument::didOpen);
	ClassDB::bind_method(D_METHOD("didChange"), &GDScriptTextDocument::didChange);
	ClassDB::bind_method(D_METHOD("documentSymbol"), &GDScriptTextDocument::documentSymbol);
	ClassDB::bind_method(D_METHOD("completion"), &GDScriptTextDocument::completion);
	ClassDB::bind_method(D_METHOD("foldingRange"), &GDScriptTextDocument::foldingRange);
	ClassDB::bind_method(D_METHOD("codeLens"), &GDScriptTextDocument::codeLens);
	ClassDB::bind_method(D_METHOD("documentLink"), &GDScriptTextDocument::documentLink);
	ClassDB::bind_method(D_METHOD("colorPresentation"), &GDScriptTextDocument::colorPresentation);
	ClassDB::bind_method(D_METHOD("hover"), &GDScriptTextDocument::hover);
}

void GDScriptTextDocument::didOpen(const Variant &p_param) {
	lsp::TextDocumentItem doc = load_document_item(p_param);
	sync_script_content(doc.uri, doc.text);
}

void GDScriptTextDocument::didChange(const Variant &p_param) {
	lsp::TextDocumentItem doc = load_document_item(p_param);
	Dictionary dict = p_param;
	Array contentChanges = dict["contentChanges"];
	for (int i = 0; i < contentChanges.size(); ++i) {
		lsp::TextDocumentContentChangeEvent evt;
		evt.load(contentChanges[i]);
		doc.text = evt.text;
	}
	sync_script_content(doc.uri, doc.text);
}

lsp::TextDocumentItem GDScriptTextDocument::load_document_item(const Variant &p_param) {
	lsp::TextDocumentItem doc;
	Dictionary params = p_param;
	doc.load(params["textDocument"]);
	print_line(doc.text);
	return doc;
}

Array GDScriptTextDocument::documentSymbol(const Dictionary &p_params) {
	Dictionary params = p_params["textDocument"];
	String path = params["uri"];
	Array arr;
	if (const Map<String, ExtendGDScriptParser *>::Element *parser = GDScriptLanguageProtocol::get_singleton()->get_workspace().scripts.find(path)) {
		Vector<lsp::SymbolInformation> list;
		parser->get()->get_symbols().symbol_tree_as_list(path, list);
		uint32_t size = list.size();
		for (size_t i = 0; i < size; i++) {
			arr.push_back(list[i].to_json());
		}
	}
	return arr;
}

Dictionary GDScriptTextDocument::completion(const Dictionary &p_params) {

	Dictionary params = p_params["position"];
	int cur_line = params["line"];
	int cur_char = params["character"];

	Vector<ScriptCodeCompletionOption> options = GDScriptLanguageProtocol::get_singleton()->get_workspace().fetch_completion(cur_line, cur_char);

	const int MAX_COMPLETION_LIMIT = 200;
	lsp::CompletionList completion_list;
	completion_list.isIncomplete = (options.size() > 0);
	for (int i = 0; i < ((options.size() > MAX_COMPLETION_LIMIT) ? MAX_COMPLETION_LIMIT : options.size()); i++) {
		lsp::CompletionItem item;
		item.label = options[i].display;
		switch (options[i].kind) {
			case ScriptCodeCompletionOption::KIND_CLASS:
				item.kind = lsp::SymbolKind::Class;
				break;
			case ScriptCodeCompletionOption::KIND_CONSTANT:
				item.kind = lsp::SymbolKind::Constant;
				break;
			case ScriptCodeCompletionOption::KIND_ENUM:
				item.kind = lsp::SymbolKind::Enum;
				break;
			case ScriptCodeCompletionOption::KIND_NODE_PATH:
			case ScriptCodeCompletionOption::KIND_FILE_PATH:
				item.kind = lsp::SymbolKind::File;
				break;
			case ScriptCodeCompletionOption::KIND_FUNCTION:
				item.kind = lsp::SymbolKind::Function;
				break;
			case ScriptCodeCompletionOption::KIND_MEMBER:
				item.kind = lsp::SymbolKind::EnumMember;
				break;
			case ScriptCodeCompletionOption::KIND_SIGNAL:
			case ScriptCodeCompletionOption::KIND_VARIABLE:
				item.kind = lsp::SymbolKind::Variable;
				break;
			default:
				break;
		}
		completion_list.items.push_back(item);
	}

	return completion_list.to_json();
}

Array GDScriptTextDocument::foldingRange(const Dictionary &p_params) {
	Dictionary params = p_params["textDocument"];
	String path = params["uri"];
	Array arr;
	return arr;
}

Array GDScriptTextDocument::codeLens(const Dictionary &p_params) {
	Array arr;
	return arr;
}

Variant GDScriptTextDocument::documentLink(const Dictionary &p_params) {
	Variant ret;
	return ret;
}

Array GDScriptTextDocument::colorPresentation(const Dictionary &p_params) {
	Array arr;
	return arr;
}

Variant GDScriptTextDocument::hover(const Dictionary &p_params) {
	Variant ret;
	return ret;
}

void GDScriptTextDocument::sync_script_content(const String &p_path, const String &p_content) {
	GDScriptLanguageProtocol::get_singleton()->get_workspace().parse_script(p_path, p_content);
}
