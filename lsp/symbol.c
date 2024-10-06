/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <list.h>
#include <log.h>
#include <lsp.h>
#include <symtab.h>
#include <utils.h>


/* global functions */
void *lsp_definition(lsp_dict_t *params){
	char const *uri;
	char name[64];
	size_t line,
		   col;
	file_t *file;
	symbol_t *sym;
	lsp_list_t *items;
	lsp_dict_t *item;


	/* parse params */
	uri = lsp_params_doc_uri(params);
	file = symtab_file_lookup(uri);

	if(file == NULL)
		return lsp_result_error("file not in symbol table");

	if(lsp_params_position(params, &line, &col) != 0)
		return lsp_result_error("invalid symbol position");

	items = lsp_list_init();

	if(file_locate_identifier(file->text, line, col, name, sizeof(name), false) != 0)
		return lsp_result_commit(items);

	/* collect items */
	VERBOSE("request: definition file=\"%s\", line=%zu, column=%zu, text=\"%s\"", uri, line, col, name);

	list_for_each(symtab_symbols(), sym){
		if(strcmp(sym->name, name) == 0){
			item = lsp_dict_init();
			lsp_dict_add_string(item, "uri", sym->file->uri);
			lsp_dict_add_dict(item, "range", lsp_fmt_range(sym->line, sym->column));

			lsp_list_add(items, item);
		}
	}

	return lsp_result_commit(items);
}

void *lsp_symbols(lsp_dict_t *params){
	char const *uri;
	file_t *file;
	symbol_t *sym;
	lsp_list_t *items;
	lsp_dict_t *item;


	/* parse params */
	uri = lsp_params_doc_uri(params);
	file = symtab_file_lookup(uri);

	if(file == NULL)
		return lsp_result_error("file not in symbol table");

	/* collect items */
	VERBOSE("request: symbols file=\"%s\"", uri);

	items = lsp_list_init();

	list_for_each(file->symbols, sym){
		item = lsp_dict_init();
		lsp_dict_add_string(item, "name", sym->name);
		lsp_dict_add_string(item, "detail", sym->signature);
		lsp_dict_add_int(item, "kind", LSP_SYM_KIND_FUNCTION);
		lsp_dict_add_dict(item, "range", lsp_fmt_range(sym->line, sym->column));
		lsp_dict_add_dict(item, "selectionRange", lsp_fmt_range(sym->line, sym->column));

		lsp_list_add(items, item);
	}

	return lsp_result_commit(items);
}
