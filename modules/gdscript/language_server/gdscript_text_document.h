#ifndef GDSCRIPT_TEXT_DOCUMENT_H
#define GDSCRIPT_TEXT_DOCUMENT_H

#include "core/reference.h"
#include "lsp.hpp"

class GDScriptTextDocument : public Reference {
	GDCLASS(GDScriptTextDocument, Reference)
protected:
	static void _bind_methods();

	void didOpen(const Variant &p_param);
	void didChange(const Variant &p_param);

	void sync_script_content(const String &p_path, const String &p_content);

private:
	lsp::TextDocumentItem load_document_item(const Variant &p_param);

public:
	Array documentSymbol(const Dictionary &p_params);
	Dictionary completion(const Dictionary &p_params);
	Array foldingRange(const Dictionary &p_params);
	Array codeLens(const Dictionary &p_params);
	Variant documentLink(const Dictionary &p_params);
	Array colorPresentation(const Dictionary &p_params);
	Variant hover(const Dictionary &p_params);
};

#endif
