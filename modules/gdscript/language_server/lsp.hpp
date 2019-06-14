#ifndef GODOT_LSP_H
#define GODOT_LSP_H

#include "core/variant.h"

namespace lsp {

typedef String DocumentUri;

/**
 * Position in a text document expressed as zero-based line and zero-based character offset.
 * A position is between two characters like an ‘insert’ cursor in a editor.
 * Special values like for example -1 to denote the end of a line are not supported.
 */
struct Position {
	/**
	 * Line position in a document (zero-based).
	 */
	int line;

	/**
	 * Character offset on a line in a document (zero-based). Assuming that the line is
	 * represented as a string, the `character` value represents the gap between the
	 * `character` and `character + 1`.
	 *
	 * If the character value is greater than the line length it defaults back to the
	 * line length.
	 */
	int character;

	Position() :
			line(0),
			character(0) {}

	void load(const Dictionary &p_params) {
		line = p_params["line"];
		character = p_params["character"];
	}

	Dictionary to_json() const {
		Dictionary dict;
		dict["line"] = line;
		dict["character"] = character;
		return dict;
	}
};

/**
 * A range in a text document expressed as (zero-based) start and end positions.
 * A range is comparable to a selection in an editor. Therefore the end position is exclusive.
 * If you want to specify a range that contains a line including the line ending character(s) then use an end position denoting the start of the next line.
 */
struct Range {
	/**
	 * The range's start position.
	 */
	Position start;

	/**
	 * The range's end position.
	 */
	Position end;

	void load(const Dictionary &p_params) {
		start.load(p_params["start"]);
		end.load(p_params["end"]);
	}

	Dictionary to_json() const {
		Dictionary dict;
		dict["start"] = start.to_json();
		dict["end"] = end.to_json();
		return dict;
	}
};

/**
 * Represents a location inside a resource, such as a line inside a text file.
 */
struct Location {
	DocumentUri uri;
	Range range;

	void load(const Dictionary &p_params) {
		uri = p_params["uri"];
		range.load(p_params["range"]);
	}

	Dictionary to_json() const {
		Dictionary dict;
		dict["uri"] = uri;
		dict["range"] = range.to_json();
		return dict;
	}
};

/**
 * Represents a link between a source and a target location.
 */
struct LocationLink {

	/**
	 * Span of the origin of this link.
	 *
	 * Used as the underlined span for mouse interaction. Defaults to the word range at
	 * the mouse position.
	 */
	Range *originSelectionRange;

	/**
	 * The target resource identifier of this link.
	 */
	String targetUri;

	/**
	 * The full target range of this link. If the target for example is a symbol then target range is the
	 * range enclosing this symbol not including leading/trailing whitespace but everything else
	 * like comments. This information is typically used to highlight the range in the editor.
	 */
	Range targetRange;

	/**
	 * The range that should be selected and revealed when this link is being followed, e.g the name of a function.
	 * Must be contained by the the `targetRange`. See also `DocumentSymbol#range`
	 */
	Range targetSelectionRange;

	LocationLink() :
			originSelectionRange(NULL) {}
};

enum TextDocumentSyncKind {
	/**
	 * Documents should not be synced at all.
	 */
	None = 0,

	/**
	 * Documents are synced by always sending the full content
	 * of the document.
	 */
	Full = 1,

	/**
	 * Documents are synced by sending the full content on open.
	 * After that only incremental updates to the document are
	 * send.
	 */
	Incremental = 2,
};

/**
 * Completion options.
 */
struct CompletionOptions {
	/**
	 * The server provides support to resolve additional
	 * information for a completion item.
	 */
	bool resolveProvider;

	/**
	 * The characters that trigger completion automatically.
	 */
	Vector<String> triggerCharacters;

	CompletionOptions() :
			resolveProvider(true) {
		triggerCharacters.push_back(".");
		triggerCharacters.push_back("$");
		triggerCharacters.push_back("'");
		triggerCharacters.push_back("\"");
		triggerCharacters.push_back("(");
		triggerCharacters.push_back(",");
	}

	Dictionary to_json() const {
		Dictionary dict;
		dict["resolveProvider"] = resolveProvider;
		dict["triggerCharacters"] = triggerCharacters;
		return dict;
	}
};

/**
 * Signature help options.
 */
struct SignatureHelpOptions {
	/**
	 * The characters that trigger signature help
	 * automatically.
	 */
	Vector<String> triggerCharacters;

	Dictionary to_json() {
		Dictionary dict;
		dict["triggerCharacters"] = triggerCharacters;
		return dict;
	}
};

/**
 * Code Lens options.
 */
struct CodeLensOptions {
	/**
	 * Code lens has a resolve provider as well.
	 */
	bool resolveProvider;

	CodeLensOptions() :
			resolveProvider(false) {}

	Dictionary to_json() {
		Dictionary dict;
		dict["resolveProvider"] = resolveProvider;
		return dict;
	}
};

/**
 * Rename options
 */
struct RenameOptions {
	/**
	 * Renames should be checked and tested before being executed.
	 */
	bool prepareProvider;

	RenameOptions() :
			prepareProvider(false) {}

	Dictionary to_json() {
		Dictionary dict;
		dict["prepareProvider"] = prepareProvider;
		return dict;
	}
};

/**
 * Document link options.
 */
struct DocumentLinkOptions {
	/**
	 * Document links have a resolve provider as well.
	 */
	bool resolveProvider;

	DocumentLinkOptions() :
			resolveProvider(false) {}

	Dictionary to_json() {
		Dictionary dict;
		dict["resolveProvider"] = resolveProvider;
		return dict;
	}
};

/**
 * Execute command options.
 */
struct ExecuteCommandOptions {
	/**
	 * The commands to be executed on the server
	 */
	Vector<String> commands;

	Dictionary to_json() {
		Dictionary dict;
		dict["commands"] = commands;
		return dict;
	}
};

/**
 * Save options.
 */
struct SaveOptions {
	/**
	 * The client is supposed to include the content on save.
	 */
	bool includeText;

	SaveOptions() :
			includeText(true) {}

	Dictionary to_json() {
		Dictionary dict;
		dict["includeText"] = includeText;
		return dict;
	}
};

/**
 * Color provider options.
 */
struct ColorProviderOptions {
	Dictionary to_json() {
		Dictionary dict;
		return dict;
	}
};

/**
 * Folding range provider options.
 */
struct FoldingRangeProviderOptions {
	Dictionary to_json() {
		Dictionary dict;
		return dict;
	}
};

struct TextDocumentSyncOptions {
	/**
	 * Open and close notifications are sent to the server. If omitted open close notification should not
	 * be sent.
	 */
	bool openClose;

	/**
	 * Change notifications are sent to the server. See TextDocumentSyncKind.None, TextDocumentSyncKind.Full
	 * and TextDocumentSyncKind.Incremental. If omitted it defaults to TextDocumentSyncKind.None.
	 */
	TextDocumentSyncKind change;

	/**
	 * If present will save notifications are sent to the server. If omitted the notification should not be
	 * sent.
	 */
	bool willSave;

	/**
	 * If present will save wait until requests are sent to the server. If omitted the request should not be
	 * sent.
	 */
	bool willSaveWaitUntil;

	/**
	 * If present save notifications are sent to the server. If omitted the notification should not be
	 * sent.
	 */
	SaveOptions save;

	TextDocumentSyncOptions() :
			openClose(true),
			willSave(false),
			willSaveWaitUntil(false) {}

	Dictionary to_json() {
		Dictionary dict;
		dict["willSaveWaitUntil"] = willSaveWaitUntil;
		dict["willSave"] = willSave;
		dict["openClose"] = openClose;
		dict["change"] = (int)change;
		dict["change"] = save.to_json();
		return dict;
	}
};

/**
 * Static registration options to be returned in the initialize request.
 */
struct StaticRegistrationOptions {
	/**
	 * The id used to register the request. The id can be used to deregister
	 * the request again. See also Registration#id.
	 */
	String id;
};

/**
 * Format document on type options.
 */
struct DocumentOnTypeFormattingOptions {
	/**
	 * A character on which formatting should be triggered, like `}`.
	 */
	String firstTriggerCharacter;

	/**
	 * More trigger characters.
	 */
	Vector<String> moreTriggerCharacter;

	Dictionary to_json() {
		Dictionary dict;
		dict["firstTriggerCharacter"] = firstTriggerCharacter;
		dict["moreTriggerCharacter"] = moreTriggerCharacter;
		return dict;
	}
};

struct TextDocumentItem {
	/**
	 * The text document's URI.
	 */
	DocumentUri uri;

	/**
	 * The text document's language identifier.
	 */
	String languageId;

	/**
	 * The version number of this document (it will increase after each
	 * change, including undo/redo).
	 */
	int version;

	/**
	 * The content of the opened text document.
	 */
	String text;

	void load(const Dictionary &p_dict) {
		uri = p_dict["uri"];
		languageId = p_dict["languageId"];
		version = p_dict["version"];
		text = p_dict["text"];
	}

	Dictionary to_json() const {
		Dictionary dict;
		dict["uri"] = uri;
		dict["languageId"] = languageId;
		dict["version"] = version;
		dict["text"] = text;
		return dict;
	}
};

/**
 * An event describing a change to a text document. If range and rangeLength are omitted
 * the new text is considered to be the full content of the document.
 */
struct TextDocumentContentChangeEvent {
	/**
	 * The range of the document that changed.
	 */
	Range range;

	/**
	 * The length of the range that got replaced.
	 */
	int rangeLength;

	/**
	 * The new text of the range/document.
	 */
	String text;

	void load(const Dictionary &p_params) {
		text = p_params["text"];
		rangeLength = p_params["rangeLength"];
		range.load(p_params["range"]);
	}
};

enum DiagnosticSeverity {
	/**
	 * Reports an error.
	 */
	Error = 1,
	/**
	 * Reports a warning.
	 */
	Warning = 2,
	/**
	 * Reports an information.
	 */
	Information = 3,
	/**
	 * Reports a hint.
	 */
	Hint = 4,
};

/**
 * Represents a related message and source code location for a diagnostic. This should be
 * used to point to code locations that cause or related to a diagnostics, e.g when duplicating
 * a symbol in a scope.
 */
struct DiagnosticRelatedInformation {
	/**
	 * The location of this related diagnostic information.
	 */
	Location location;

	/**
	 * The message of this related diagnostic information.
	 */
	String message;

	Dictionary to_json() const {
		Dictionary dict;
		dict["location"] = location.to_json(),
		dict["message"] = message;
		return dict;
	}
};

/**
 * Represents a diagnostic, such as a compiler error or warning.
 * Diagnostic objects are only valid in the scope of a resource.
 */
struct Diagnostic {
	/**
	 * The range at which the message applies.
	 */
	Range range;

	/**
	 * The diagnostic's severity. Can be omitted. If omitted it is up to the
	 * client to interpret diagnostics as error, warning, info or hint.
	 */
	DiagnosticSeverity severity;

	/**
	 * The diagnostic's code, which might appear in the user interface.
	 */
	int code;

	/**
	 * A human-readable string describing the source of this
	 * diagnostic, e.g. 'typescript' or 'super lint'.
	 */
	String source;

	/**
	 * The diagnostic's message.
	 */
	String message;

	/**
	 * An array of related diagnostic information, e.g. when symbol-names within
	 * a scope collide all definitions can be marked via this property.
	 */
	Vector<DiagnosticRelatedInformation> relatedInformation;

	Dictionary to_json() const {
		Dictionary dict;
		dict["range"] = range.to_json();
		dict["code"] = code;
		dict["severity"] = severity;
		dict["message"] = message;
		dict["source"] = source;
		if (!relatedInformation.empty()) {
			Array arr;
			arr.resize(relatedInformation.size());
			for (int i = 0; i < relatedInformation.size(); i++) {
				arr[i] = relatedInformation[i].to_json();
			}
			dict["relatedInformation"] = arr;
		}
		return dict;
	}
};

/**
 * A symbol kind.
 */
namespace SymbolKind {
static const int File = 1;
static const int Module = 2;
static const int Namespace = 3;
static const int Package = 4;
static const int Class = 5;
static const int Method = 6;
static const int Property = 7;
static const int Field = 8;
static const int Constructor = 9;
static const int Enum = 10;
static const int Interface = 11;
static const int Function = 12;
static const int Variable = 13;
static const int Constant = 14;
static const int String = 15;
static const int Number = 16;
static const int Boolean = 17;
static const int Array = 18;
static const int Object = 19;
static const int Key = 20;
static const int Null = 21;
static const int EnumMember = 22;
static const int Struct = 23;
static const int Event = 24;
static const int Operator = 25;
static const int TypeParameter = 26;
}; // namespace SymbolKind

/**
 * Represents information about programming constructs like variables, classes,
 * interfaces etc.
 */
struct SymbolInformation {
	/**
	 * The name of this symbol.
	 */
	String name;

	/**
	 * The kind of this symbol.
	 */
	int kind;

	/**
	 * Indicates if this symbol is deprecated.
	 */
	bool deprecated;

	/**
	 * The location of this symbol. The location's range is used by a tool
	 * to reveal the location in the editor. If the symbol is selected in the
	 * tool the range's start information is used to position the cursor. So
	 * the range usually spans more then the actual symbol's name and does
	 * normally include things like visibility modifiers.
	 *
	 * The range doesn't have to denote a node range in the sense of a abstract
	 * syntax tree. It can therefore not be used to re-construct a hierarchy of
	 * the symbols.
	 */
	Location location;

	/**
	 * The name of the symbol containing this symbol. This information is for
	 * user interface purposes (e.g. to render a qualifier in the user interface
	 * if necessary). It can't be used to re-infer a hierarchy for the document
	 * symbols.
	 */
	String containerName;

	SymbolInformation() :
			kind(SymbolKind::File),
			deprecated(false) {}

	Dictionary to_json() const {
		Dictionary dict;
		dict["name"] = name;
		dict["kind"] = kind;
		dict["deprecated"] = deprecated;
		dict["location"] = location.to_json();
		dict["containerName"] = containerName;
		return dict;
	}
};

/**
 * Represents programming constructs like variables, classes, interfaces etc. that appear in a document. Document symbols can be
 * hierarchical and they have two ranges: one that encloses its definition and one that points to its most interesting range,
 * e.g. the range of an identifier.
 */
struct DocumentSymbol {

	/**
	 * The name of this symbol. Will be displayed in the user interface and therefore must not be
	 * an empty string or a string only consisting of white spaces.
	 */
	String name;

	/**
	 * More detail for this symbol, e.g the signature of a function.
	 */
	String detail;

	/**
	 * The kind of this symbol.
	 */
	int kind;

	/**
	 * Indicates if this symbol is deprecated.
	 */
	bool deprecated;

	/**
	 * The range enclosing this symbol not including leading/trailing whitespace but everything else
	 * like comments. This information is typically used to determine if the clients cursor is
	 * inside the symbol to reveal in the symbol in the UI.
	 */
	Range range;

	/**
	 * The range that should be selected and revealed when this symbol is being picked, e.g the name of a function.
	 * Must be contained by the `range`.
	 */
	Range selectionRange;

	/**
	 * Children of this symbol, e.g. properties of a class.
	 */
	Vector<DocumentSymbol> children;

	DocumentSymbol() :
			kind(SymbolKind::File),
			deprecated(false) {}

	Dictionary to_json() const {
		Dictionary dict;
		dict["name"] = name;
		dict["detail"] = detail;
		dict["kind"] = kind;
		dict["deprecated"] = deprecated;
		dict["range"] = range.to_json();
		dict["selectionRange"] = selectionRange.to_json();
		Array arr;
		arr.resize(children.size());
		uint32_t size = children.size();
		for (size_t i = 0; i < size; i++) {
			arr[i] = children[i].to_json();
		}
		dict["children"] = arr;
		return dict;
	}

	void symbol_tree_as_list(const String &p_uri, Vector<SymbolInformation> &r_list, const String &p_container = "") const {
		SymbolInformation si;
		si.name = name;
		si.kind = kind;
		si.containerName = p_container;
		si.deprecated = deprecated;
		si.location.uri = p_uri;
		si.location.range = range;
		r_list.push_back(si);
		uint32_t size = children.size();
		for (size_t i = 0; i < size; i++) {
			children[i].symbol_tree_as_list(p_uri, r_list, name);
		}
	}
};

/**
 * A textual edit applicable to a text document.
 */
struct TextEdit {
	/**
	 * The range of the text document to be manipulated. To insert
	 * text into a document create a range where start === end.
	 */
	Range range;

	/**
	 * The string to be inserted. For delete operations use an
	 * empty string.
	 */
	String newText;
};

/**
 * Represents a reference to a command.
 * Provides a title which will be used to represent a command in the UI.
 * Commands are identified by a string identifier.
 * The recommended way to handle commands is to implement their execution on the server side if the client and server provides the corresponding capabilities.
 * Alternatively the tool extension code could handle the command. The protocol currently doesn’t specify a set of well-known commands.
 */
struct Command {
	/**
	 * Title of the command, like `save`.
	 */
	String title;
	/**
	 * The identifier of the actual command handler.
	 */
	String command;
	/**
	 * Arguments that the command handler should be
	 * invoked with.
	 */
	Array arguments;

	Dictionary to_json() const {
		Dictionary dict;
		dict["title"] = title;
		dict["command"] = command;
		if (arguments.size()) dict["arguments"] = arguments;
		return dict;
	}
};

/**
 * The kind of a completion entry.
 */
namespace CompletionItemKind {
static const int Text = 1;
static const int Method = 2;
static const int Function = 3;
static const int Constructor = 4;
static const int Field = 5;
static const int Variable = 6;
static const int Class = 7;
static const int Interface = 8;
static const int Module = 9;
static const int Property = 10;
static const int Unit = 11;
static const int Value = 12;
static const int Enum = 13;
static const int Keyword = 14;
static const int Snippet = 15;
static const int Color = 16;
static const int File = 17;
static const int Reference = 18;
static const int Folder = 19;
static const int EnumMember = 20;
static const int Constant = 21;
static const int Struct = 22;
static const int Event = 23;
static const int Operator = 24;
static const int TypeParameter = 25;
}; // namespace CompletionItemKind

/**
 * Defines whether the insert text in a completion item should be interpreted as
 * plain text or a snippet.
 */
enum InsertTextFormat {
	/**
	 * The primary text to be inserted is treated as a plain string.
	 */
	PlainText = 1,

	/**
	 * The primary text to be inserted is treated as a snippet.
	 *
	 * A snippet can define tab stops and placeholders with `$1`, `$2`
	 * and `${3:foo}`. `$0` defines the final tab stop, it defaults to
	 * the end of the snippet. Placeholders with equal identifiers are linked,
	 * that is typing in one will update others too.
	 */
	Snippet = 2
};

struct CompletionItem {
	/**
	 * The label of this completion item. By default
	 * also the text that is inserted when selecting
	 * this completion.
	 */
	String label;

	/**
	 * The kind of this completion item. Based of the kind
	 * an icon is chosen by the editor. The standardized set
	 * of available values is defined in `CompletionItemKind`.
	 */
	int kind;

	/**
	 * A human-readable string with additional information
	 * about this item, like type or symbol information.
	 */
	String detail;

	/**
	 * A human-readable string that represents a doc-comment.
	 */
	String documentation;

	/**
	 * Indicates if this item is deprecated.
	 */
	bool deprecated;

	/**
	 * Select this item when showing.
	 *
	 * *Note* that only one completion item can be selected and that the
	 * tool / client decides which item that is. The rule is that the *first*
	 * item of those that match best is selected.
	 */
	bool preselect;

	/**
	 * A string that should be used when comparing this item
	 * with other items. When `falsy` the label is used.
	 */
	String sortText;

	/**
	 * A string that should be used when filtering a set of
	 * completion items. When `falsy` the label is used.
	 */
	String filterText;

	/**
	 * A string that should be inserted into a document when selecting
	 * this completion. When `falsy` the label is used.
	 *
	 * The `insertText` is subject to interpretation by the client side.
	 * Some tools might not take the string literally. For example
	 * VS Code when code complete is requested in this example `con<cursor position>`
	 * and a completion item with an `insertText` of `console` is provided it
	 * will only insert `sole`. Therefore it is recommended to use `textEdit` instead
	 * since it avoids additional client side interpretation.
	 *
	 * @deprecated Use textEdit instead.
	 */
	String insertText;

	/**
	 * The format of the insert text. The format applies to both the `insertText` property
	 * and the `newText` property of a provided `textEdit`.
	 */
	InsertTextFormat insertTextFormat;

	/**
	 * An edit which is applied to a document when selecting this completion. When an edit is provided the value of
	 * `insertText` is ignored.
	 *
	 * *Note:* The range of the edit must be a single line range and it must contain the position at which completion
	 * has been requested.
	 */
	TextEdit textEdit;

	/**
	 * An optional array of additional text edits that are applied when
	 * selecting this completion. Edits must not overlap (including the same insert position)
	 * with the main edit nor with themselves.
	 *
	 * Additional text edits should be used to change text unrelated to the current cursor position
	 * (for example adding an import statement at the top of the file if the completion item will
	 * insert an unqualified type).
	 */
	Vector<TextEdit> additionalTextEdits;

	/**
	 * An optional set of characters that when pressed while this completion is active will accept it first and
	 * then type that character. *Note* that all commit characters should have `length=1` and that superfluous
	 * characters will be ignored.
	 */
	Vector<String> commitCharacters;

	/**
	 * An optional command that is executed *after* inserting this completion. *Note* that
	 * additional modifications to the current document should be described with the
	 * additionalTextEdits-property.
	 */
	Command command;

	/**
	 * A data entry field that is preserved on a completion item between
	 * a completion and a completion resolve request.
	 */
	Variant data;

	CompletionItem() :
			deprecated(false),
			preselect(false) {}

	Dictionary to_json() const {
		Dictionary dict;
		dict["label"] = label;
		dict["kind"] = kind;
		dict["detail"] = detail;
		dict["documentation"] = documentation;
		dict["deprecated"] = deprecated;
		dict["preselect"] = preselect;
		dict["sortText"] = sortText;
		dict["filterText"] = filterText;
		dict["insertText"] = insertText;
		if (commitCharacters.size()) dict["commitCharacters"] = commitCharacters;
		dict["command"] = command.to_json();
		dict["data"] = data;
		return dict;
	}
};

/**
 * Represents a collection of [completion items](#CompletionItem) to be presented
 * in the editor.
 */
struct CompletionList {
	/**
	 * This list it not complete. Further typing should result in recomputing
	 * this list.
	 */
	bool isIncomplete;

	/**
	 * The completion items.
	 */
	Vector<CompletionItem> items;

	Dictionary to_json() const {
		Dictionary dict;
		dict["isIncomplete"] = isIncomplete;
		if (!items.empty()) {
			Array arr;
			arr.resize(items.size());
			for (int i = 0; i < items.size(); i++) {
				arr[i] = items[i].to_json();
			}
			dict["items"] = arr;
		}
		return dict;
	}
};

/**
 * Enum of known range kinds
 */
namespace FoldingRangeKind {
/**
	 * Folding range for a comment
	 */
static const String Comment = "comment";
/**
	 * Folding range for a imports or includes
	 */
static const String Imports = "imports";
/**
	 * Folding range for a region (e.g. `#region`)
	 */
static const String Region = "region";
} // namespace FoldingRangeKind

/**
 * Represents a folding range.
 */
struct FoldingRange {

	/**
	 * The zero-based line number from where the folded range starts.
	 */
	int startLine;

	/**
	 * The zero-based character offset from where the folded range starts. If not defined, defaults to the length of the start line.
	 */
	int startCharacter;

	/**
	 * The zero-based line number where the folded range ends.
	 */
	int endLine;

	/**
	 * The zero-based character offset before the folded range ends. If not defined, defaults to the length of the end line.
	 */
	int endCharacter;

	/**
	 * Describes the kind of the folding range such as `comment' or 'region'. The kind
	 * is used to categorize folding ranges and used by commands like 'Fold all comments'. See
	 * [FoldingRangeKind](#FoldingRangeKind) for an enumeration of standardized kinds.
	 */
	String kind;

	FoldingRange() :
			startLine(0),
			startCharacter(0),
			endLine(0),
			endCharacter(0) {
		kind = FoldingRangeKind::Region;
	}

	Dictionary to_json() const {
		Dictionary dict;
		dict["startLine"] = startLine;
		dict["startCharacter"] = startCharacter;
		dict["endLine"] = endLine;
		dict["endCharacter"] = endCharacter;
		return dict;
	}
};

struct ServerCapabilities {
	/**
	 * Defines how text documents are synced. Is either a detailed structure defining each notification or
	 * for backwards compatibility the TextDocumentSyncKind number. If omitted it defaults to `TextDocumentSyncKind.None`.
	 */
	TextDocumentSyncOptions textDocumentSync;

	/**
	 * The server provides hover support.
	 */
	bool hoverProvider;

	/**
	 * The server provides completion support.
	 */
	CompletionOptions completionProvider;

	/**
	 * The server provides signature help support.
	 */
	SignatureHelpOptions signatureHelpProvider;

	/**
	 * The server provides goto definition support.
	 */
	bool definitionProvider;

	/**
	 * The server provides Goto Type Definition support.
	 *
	 * Since 3.6.0
	 */
	bool typeDefinitionProvider;

	/**
	 * The server provides Goto Implementation support.
	 *
	 * Since 3.6.0
	 */
	bool implementationProvider;

	/**
	 * The server provides find references support.
	 */
	bool referencesProvider;

	/**
	 * The server provides document highlight support.
	 */
	bool documentHighlightProvider;

	/**
	 * The server provides document symbol support.
	 */
	bool documentSymbolProvider;

	/**
	 * The server provides workspace symbol support.
	 */
	bool workspaceSymbolProvider;

	/**
	 * The server provides code actions. The `CodeActionOptions` return type is only
	 * valid if the client signals code action literal support via the property
	 * `textDocument.codeAction.codeActionLiteralSupport`.
	 */
	bool codeActionProvider;

	/**
	 * The server provides code lens.
	 */
	CodeLensOptions codeLensProvider;

	/**
	 * The server provides document formatting.
	 */
	bool documentFormattingProvider;

	/**
	 * The server provides document range formatting.
	 */
	bool documentRangeFormattingProvider;

	/**
	 * The server provides document formatting on typing.
	 */
	DocumentOnTypeFormattingOptions documentOnTypeFormattingProvider;

	/**
	 * The server provides rename support. RenameOptions may only be
	 * specified if the client states that it supports
	 * `prepareSupport` in its initial `initialize` request.
	 */
	RenameOptions renameProvider;

	/**
	 * The server provides document link support.
	 */
	DocumentLinkOptions documentLinkProvider;

	/**
	 * The server provides color provider support.
	 *
	 * Since 3.6.0
	 */
	ColorProviderOptions colorProvider;

	/**
	 * The server provides folding provider support.
	 *
	 * Since 3.10.0
	 */
	FoldingRangeProviderOptions foldingRangeProvider;

	/**
	 * The server provides go to declaration support.
	 *
	 * Since 3.14.0
	 */
	bool declarationProvider;

	/**
	 * The server provides execute command support.
	 */
	ExecuteCommandOptions executeCommandProvider;

	ServerCapabilities() :
			hoverProvider(true),
			definitionProvider(false),
			typeDefinitionProvider(false),
			implementationProvider(false),
			referencesProvider(false),
			documentHighlightProvider(false),
			documentSymbolProvider(false),
			workspaceSymbolProvider(true),
			codeActionProvider(false),
			documentRangeFormattingProvider(false),
			declarationProvider(true) {}

	Dictionary to_json() {
		Dictionary dict;
		dict["textDocumentSync"] = (int)textDocumentSync.change;
		dict["completionProvider"] = completionProvider.to_json();
		dict["signatureHelpProvider"] = signatureHelpProvider.to_json();
		dict["codeLensProvider"] = false; // codeLensProvider.to_json();
		dict["documentOnTypeFormattingProvider"] = documentOnTypeFormattingProvider.to_json();
		dict["renameProvider"] = renameProvider.to_json();
		dict["documentLinkProvider"] = documentLinkProvider.to_json();
		dict["colorProvider"] = false; // colorProvider.to_json();
		dict["foldingRangeProvider"] = false; //foldingRangeProvider.to_json();
		dict["executeCommandProvider"] = executeCommandProvider.to_json();
		dict["hoverProvider"] = hoverProvider;
		dict["definitionProvider"] = definitionProvider;
		dict["typeDefinitionProvider"] = typeDefinitionProvider;
		dict["implementationProvider"] = implementationProvider;
		dict["referencesProvider"] = referencesProvider;
		dict["documentHighlightProvider"] = documentHighlightProvider;
		dict["documentSymbolProvider"] = documentSymbolProvider;
		dict["workspaceSymbolProvider"] = workspaceSymbolProvider;
		dict["codeActionProvider"] = codeActionProvider;
		dict["documentFormattingProvider"] = documentFormattingProvider;
		dict["documentRangeFormattingProvider"] = documentRangeFormattingProvider;
		dict["declarationProvider"] = declarationProvider;
		return dict;
	}
};

struct InitializeResult {
	/**
	 * The capabilities the language server provides.
	 */
	ServerCapabilities capabilities;

	Dictionary to_json() {
		Dictionary dict;
		dict["capabilities"] = capabilities.to_json();
		return dict;
	}
};

} // namespace lsp

#endif
