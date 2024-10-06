/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <log.h>
#include <lsp.h>
#include <symtab.h>
#include <utils.h>


/* local/static prototypes */
static void match_symbols(char const *name, file_t *file, lsp_list_t *items);


/* global functions */
void *lsp_completion(lsp_dict_t *params){
	char const *uri;
	char name[64];
	size_t line,
		   col;
	file_t *file,
		   *hdr;
	lsp_list_t *items;


	/* parse params */
	uri = lsp_params_doc_uri(params);
	file = symtab_file_lookup(uri);

	if(file == NULL)
		return lsp_result_error("file not in symbol table");

	if(lsp_params_position(params, &line, &col) != 0)
		return lsp_result_error("invalid symbol position");

	items = lsp_list_init();

	if(file_locate_identifier(file->text, line, col, name, sizeof(name), true) != 0)
		return lsp_result_commit(items);

	/* collect completion items */
	VERBOSE("request: completion file=\"%s\", line=%zu column=%zu, text=\"%s\"", uri, line, col, name);

	match_symbols(name, file, items);

	list_for_each(file->header, hdr)
		match_symbols(name, hdr, items);

	return lsp_result_commit(items);
}


/* local functions */
static void match_symbols(char const *name, file_t *file, lsp_list_t *items){
	size_t name_len = strlen(name);
	symbol_t *sym;
	lsp_dict_t *item;


	list_for_each(file->symbols, sym){
		if(strncmp(sym->name, name, name_len) == 0){
			item = lsp_dict_init();
			lsp_dict_add_string(item, "label", sym->signature);
			lsp_dict_add_string(item, "insertText", sym->name);
			lsp_dict_add_int(item, "kind", LSP_COMPL_KIND_FUNCTION);

			lsp_list_add(items, item);
		}
	}
}
