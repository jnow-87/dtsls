/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <log.h>
#include <lsp.h>
#include <symtab.h>


/* global functions */
void *lsp_doc_open(lsp_dict_t *params){
	char const *path,
			   *text;


	path = lsp_uri_to_path(lsp_params_doc_uri(params));
	text = lsp_params_doc_text(params);

	VERBOSE("notification: open file=\"%s\"", path);
	symtab_update(path, strlen(path), text, strlen(text));

	return NULL;
}

void *lsp_doc_changed(lsp_dict_t *params){
	char const *path,
			   *text;


	path = lsp_uri_to_path(lsp_params_doc_uri(params));
	text = lsp_dict_get_string(lsp_list_get_idx(lsp_dict_get_list(params, "contentChanges"), 0), "text");

	VERBOSE("notification: changed file=\"%s\"", path);
	symtab_update(path, strlen(path), text, strlen(text));

	return NULL;
}

