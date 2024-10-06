/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdbool.h>
#include <lsp.h>


/* global functions */
void *lsp_initialize(lsp_dict_t *params){
	lsp_dict_t *result,
			   *capa,
			   *sync;


	capa = lsp_dict_init();

	sync = lsp_dict_init();
	lsp_dict_add_bool(sync, "openClose", true);
	lsp_dict_add_int(sync, "change", 1);
	lsp_dict_add_dict(capa, "textDocumentSync", sync);

	lsp_dict_add_bool(capa, "definitionProvider", true);
	lsp_dict_add_bool(capa, "documentSymbolProvider", true);
	lsp_dict_add_dict(capa, "completionProvider", lsp_dict_init());

	result = lsp_dict_init();
	lsp_dict_add_dict(result, "capabilities", capa);

	return lsp_result_commit(result);
}

void *lsp_shutdown(lsp_dict_t *params){
	return lsp_result_commit(NULL);
}

void *lsp_exit(lsp_dict_t *params){
	return lsp_result_exit();
}
