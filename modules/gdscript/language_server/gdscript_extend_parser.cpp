#include "gdscript_extend_parser.h"
#include "../gdscript.h"

void ExtendGDScriptParser::update_diagnostics() {

	diagnostics.clear();

	if (has_error()) {
		lsp::Diagnostic diagnostic;
		diagnostic.severity = lsp::DiagnosticSeverity::Error;
		diagnostic.message = get_error();
		diagnostic.source = "gdscript";
		diagnostic.code = -1;
		lsp::Range range;
		lsp::Position pos;
		int line = get_error_line() - 1;
		const String &line_text = get_lines()[line];
		pos.line = line;
		pos.character = line_text.length() - line_text.strip_edges(true, false).length();
		range.start = pos;
		range.end = range.start;
		range.end.character = line_text.strip_edges(false).length();
		diagnostic.range = range;
		diagnostics.push_back(diagnostic);
	}

	const List<GDScriptWarning> &warnings = get_warnings();
	for (const List<GDScriptWarning>::Element *E = warnings.front(); E; E = E->next()) {
		const GDScriptWarning &warning = E->get();
		lsp::Diagnostic diagnostic;
		diagnostic.severity = lsp::DiagnosticSeverity::Warning;
		diagnostic.message = warning.get_message();
		diagnostic.source = "gdscript";
		diagnostic.code = warning.code;
		lsp::Range range;
		lsp::Position pos;
		int line = warning.line - 1;
		const String &line_text = get_lines()[line];
		pos.line = line;
		pos.character = line_text.length() - line_text.strip_edges(true, false).length();
		range.start = pos;
		range.end = pos;
		range.end.character = line_text.strip_edges(false).length();
		diagnostic.range = range;
		diagnostics.push_back(diagnostic);
	}
}

void ExtendGDScriptParser::update_symbols() {
	const GDScriptParser::Node *head = get_parse_tree();
	if (const GDScriptParser::ClassNode *gdclass = dynamic_cast<const GDScriptParser::ClassNode *>(head)) {
		parse_class_symbol(gdclass, class_symbol);
	}
}

void ExtendGDScriptParser::parse_class_symbol(const GDScriptParser::ClassNode *p_class, lsp::DocumentSymbol &r_symbol) {
	r_symbol.children.clear();
	r_symbol.name = p_class->name;
	if (r_symbol.name.empty())
		r_symbol.name = path.get_file();
	r_symbol.kind = lsp::SymbolKind::Class;
	r_symbol.detail = p_class->get_datatype().to_string();
	r_symbol.deprecated = false;
	r_symbol.range.start.line = p_class->line - 1;
	r_symbol.range.start.character = p_class->column;
	r_symbol.range.end.line = p_class->end_line - 1;
	r_symbol.selectionRange.start.line = r_symbol.range.start.line;

	for (int i = 0; i < p_class->variables.size(); ++i) {

		const GDScriptParser::ClassNode::Member &m = p_class->variables[i];

		lsp::DocumentSymbol symbol;
		symbol.name = m.identifier;
		symbol.kind = lsp::SymbolKind::Variable;
		symbol.detail = m.data_type.to_string();
		symbol.deprecated = false;
		const int line = m.line - 1;
		symbol.range.start.line = line;
		symbol.range.start.character = lines[line].length() - lines[line].strip_edges(true, false).length();
		symbol.range.end.line = line;
		symbol.range.end.character = lines[line].length();
		symbol.selectionRange.start.line = symbol.range.start.line;

		r_symbol.children.push_back(symbol);
	}

	for (int i = 0; i < p_class->_signals.size(); ++i) {
		const GDScriptParser::ClassNode::Signal &signal = p_class->_signals[i];

		lsp::DocumentSymbol symbol;
		symbol.name = signal.name;
		symbol.kind = lsp::SymbolKind::Event;
		symbol.deprecated = false;
		const int line = signal.line - 1;
		symbol.range.start.line = line;
		symbol.range.start.character = lines[line].length() - lines[line].strip_edges(true, false).length();
		symbol.range.end.line = symbol.range.start.line;
		symbol.range.end.character = lines[line].length();
		symbol.selectionRange.start.line = symbol.range.start.line;

		r_symbol.children.push_back(symbol);
	}

	for (Map<StringName, GDScriptParser::ClassNode::Constant>::Element *E = p_class->constant_expressions.front(); E; E = E->next()) {
		lsp::DocumentSymbol symbol;
		symbol.name = E->key();
		symbol.kind = lsp::SymbolKind::Constant;
		symbol.deprecated = false;
		const int line = E->get().expression->line - 1;
		symbol.range.start.line = line;
		symbol.range.start.character = E->get().expression->column;
		symbol.range.end.line = symbol.range.start.line;
		symbol.range.end.character = lines[line].length();
		symbol.selectionRange.start.line = symbol.range.start.line;

		r_symbol.children.push_back(symbol);
	}

	for (int i = 0; i < p_class->functions.size(); ++i) {
		const GDScriptParser::FunctionNode *func = p_class->functions[i];

		lsp::DocumentSymbol symbol;
		symbol.name = func->name;
		symbol.kind = lsp::SymbolKind::Method;
		symbol.detail = func->get_datatype().to_string();
		symbol.deprecated = false;
		const int line = func->line - 1;
		symbol.range.start.line = line;
		symbol.range.start.character = func->column;
		symbol.range.end.line = MAX(func->body->end_line - 2, func->body->line);
		symbol.selectionRange.start.line = symbol.range.start.line;

		r_symbol.children.push_back(symbol);
	}

	for (int i = 0; i < p_class->static_functions.size(); ++i) {
		const GDScriptParser::FunctionNode *func = p_class->static_functions[i];

		lsp::DocumentSymbol symbol;
		symbol.name = func->name;
		symbol.kind = lsp::SymbolKind::Function;
		symbol.detail = func->get_datatype().to_string();
		symbol.deprecated = false;
		const int line = func->line - 1;
		symbol.range.start.line = line;
		symbol.range.start.character = func->column;
		symbol.range.end.line = MAX(func->body->end_line - 2, func->body->line);
		symbol.selectionRange.start.line = symbol.range.start.line;

		r_symbol.children.push_back(symbol);
	}

	for (int i = 0; i < p_class->subclasses.size(); ++i) {
		const GDScriptParser::ClassNode *subclass = p_class->subclasses[i];
		lsp::DocumentSymbol symbol;
		parse_class_symbol(subclass, symbol);
		r_symbol.children.push_back(symbol);
	}
}

Error ExtendGDScriptParser::parse(const String &p_code, const String &p_path) {
	path = p_path;
	code = p_code;
	lines = p_code.split("\n");

	Error err = GDScriptParser::parse(p_code, "", false, "", false, NULL, false);
	update_diagnostics();
	update_symbols();

	return err;
}
