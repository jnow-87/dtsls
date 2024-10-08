/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdio.h>
#include <ctype.h>
#include <json-c/json.h>
#include <lsp/cmds.hash.h>
#include <log.h>
#include <lsp.h>
#include <utils.h>


/* macros */
#define LSP_EXIT_INDICATOR	((void*)-1)


/* local/static prototypes */
static int read_header(size_t *len);
static lsp_dict_t *read_content();
static void send_response(lsp_dict_t *result, int id);


/* global functions */
int lsp_cmd_process(){
	char const *method;
	int id;
	void *result;
	lsp_cmd_t const *cmd;
	lsp_dict_t *content,
			   *params;


	content = read_content();

	if(content == NULL)
		return -ERROR("parsing lsp json packet");

	method = lsp_dict_get_string(content, "method");
	params = lsp_dict_get_dict(content, "params");
	id = lsp_dict_has(content, "id") ? lsp_dict_get_int(content, "id") : -1;
	cmd = lsp_cmds_lookup(method, strlen(method));

	VERBOSE("lsp-cmd: id=%d, method=%s, has-hdlr=%d, has-params=%d", id, method, (cmd != NULL), (params != NULL));
	result = (cmd != NULL) ? cmd->hdlr(params) : NULL;

	if(result != NULL && result != LSP_EXIT_INDICATOR){
		if(id >= 0)
			send_response(result, id);

		lsp_dict_free(result);
	}

	lsp_dict_free(content);

	return (result == LSP_EXIT_INDICATOR);
}

void *lsp_result_error(char const *msg){
	lsp_dict_t *res;


	res = lsp_dict_init();
	lsp_dict_add_string(res, "error", msg);
	ERROR("lsp handler failed with %s", msg);

	return res;
}

void *lsp_result_commit(void *result){
	lsp_dict_t *res;


	res = lsp_dict_init();
	lsp_dict_add_dict(res, "result", result);

	return res;
}

void *lsp_result_exit(){
	return LSP_EXIT_INDICATOR;
}

lsp_dict_t *lsp_fmt_range(size_t line, size_t column){
	lsp_dict_t *range,
			   *pos;


	pos = lsp_dict_init();
	lsp_dict_add_int(pos, "line", line - 1); // lsp lines are zero-based
	lsp_dict_add_int(pos, "character", column);

	range = lsp_dict_init();
	lsp_dict_add_dict(range, "start", pos);
	lsp_dict_add_dict(range, "end", pos);

	return range;
}

char const *lsp_params_doc_uri(lsp_dict_t *params){
	return lsp_dict_get_string(lsp_dict_get_dict(params, "textDocument"), "uri");
}

char const *lsp_params_doc_text(lsp_dict_t *params){
	return lsp_dict_get_string(lsp_dict_get_dict(params, "textDocument"), "text");
}

int lsp_params_position(lsp_dict_t * params, size_t *line, size_t *column){
	lsp_dict_t *pos;


	pos = lsp_dict_get_dict(params, "position");

	if(pos == NULL || !lsp_dict_has(pos, "line") || !lsp_dict_has(pos, "character"))
		return -1;

	*line = lsp_dict_get_int(pos, "line") + 1;			// lsp lines are zero-based
	*column = lsp_dict_get_int(pos, "character") + 1;	// lsp columns are zero-based

	return 0;
}

char const *lsp_uri_to_path(char const *uri){
	if(uri != NULL && strncmp(uri, "file://", 7) == 0)
		return uri + 7;

	return uri;
}

lsp_dict_t *lsp_dict_init(){
	return json_object_new_object();
}

void lsp_dict_free(lsp_dict_t *dict){
	json_object_put(dict);
}

int lsp_dict_has(lsp_dict_t *dict, char const *key){
	return json_object_object_get_ex(dict, key, NULL);
}

int lsp_dict_get_int(lsp_dict_t *dict, char const *key){
	return json_object_get_int(json_object_object_get(dict, key));
}

bool lsp_dict_get_bool(lsp_dict_t *dict, char const *key){
	return json_object_get_boolean(json_object_object_get(dict, key));
}

char const *lsp_dict_get_string(lsp_dict_t *dict, char const *key){
	return json_object_get_string(json_object_object_get(dict, key));
}

lsp_dict_t *lsp_dict_get_dict(lsp_dict_t *dict, char const *key){
	return json_object_object_get(dict, key);
}

lsp_dict_t *lsp_dict_get_list(lsp_dict_t *dict, char const *key){
	return json_object_object_get(dict, key);
}

int lsp_dict_add_int(lsp_dict_t *dict, char const *key, int val){
	return json_object_object_add(dict, key, json_object_new_int(val));
}

int lsp_dict_add_bool(lsp_dict_t *dict, char const *key, bool val){
	return json_object_object_add(dict, key, json_object_new_boolean(val));
}

int lsp_dict_add_string(lsp_dict_t *dict, char const *key, char const *val){
	return json_object_object_add(dict, key, json_object_new_string(val));
}

int lsp_dict_add_dict(lsp_dict_t *dict, char const *key, lsp_dict_t *val){
	return json_object_object_add(dict, key, val);
}

lsp_list_t *lsp_list_init(){
	return json_object_new_array();
}

size_t lsp_list_get_len(lsp_list_t *lst){
	return json_object_array_length(lst);
}

lsp_dict_t *lsp_list_get_idx(lsp_list_t *lst, size_t idx){
		return json_object_array_get_idx(lst, idx);
}

int lsp_list_add(lsp_list_t *lst, lsp_dict_t *dict){
	return json_object_array_add(lst, dict);
}


/* local functions */
static int read_header(size_t *len){
	char c;
	int r;


	/* match header start */
	r = strmatch("Content-Length: ", stdin);

	if(r != 0)
		return r;

	/* read length */
	*len = 0;

	while(1){
		if(fread(&c, 1, 1, stdin) != 1)
			return -1;

		if(c == '\r')
			break;

		if(!isdigit(c))
			return 1;

		*len = *len * 10 + c - '0';
	}

	/* match header end */
	return strmatch("\n\r\n", stdin);
}

static lsp_dict_t *read_content(){
	static char *content = NULL;
	static size_t csize = 0;
	int r = 1;
	size_t len;


	while(r > 0){
		r = read_header(&len);

		if(r < 0)
			goto err;
	}

	if(len + 1 > csize){
		free(content);
		content = malloc(len + 1);
		csize = len + 1;

		if(content == NULL)
			goto err;
	}

	len = fread(content, 1, len, stdin);
	(content)[len] = 0;

	return json_tokener_parse(content);


err:
	free(content);
	csize = 0;

	return NULL;
}

static void send_response(lsp_dict_t *result, int id){
	lsp_dict_t *cp = NULL;
	char const *s;
	lsp_dict_t *err;
	lsp_dict_t *resp;


	resp = lsp_dict_init();
	lsp_dict_add_int(resp, "id", id);
	lsp_dict_add_string(resp, "jsonrpc", "2.0");

	if(!lsp_dict_has(result, "result")){
		err = lsp_dict_init();
		s = lsp_dict_get_string(result, "error");

		lsp_dict_add_string(err, "message", (s != NULL) ? s : "unknown");
		lsp_dict_add_int(err, "code", -1);
		lsp_dict_add_dict(resp, "error", err);
	}
	else{
		json_object_deep_copy(json_object_get(lsp_dict_get_dict(result, "result")), &cp, NULL);
		lsp_dict_add_dict(resp, "result", cp);
	}

	s = json_object_to_json_string(resp);

	VERBOSE("response: %s", s);
	printf("Content-Length: %zu\r\n\r\n%s", strlen(s), s);
	fflush(stdout);

	lsp_dict_free(resp);
}
