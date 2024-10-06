/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */


#include <config/config.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <json-c/json.h>
#include <config.h>
#include <log.h>
#include <utils.h>


/* global variables */
config_t config = {
	.cwd = { .path = NULL, .fd = -1},
	.include_dirs = NULL,
	.nincludes = 0,
	.log_file = NULL,
	.verbose = false,
};


/* global functions */
int config_init(){
	char const *text;
	json_object *jobj,
				*jarr;


	/* set cwd */
	config.cwd.path = getcwd(NULL, 0);

	if(config.cwd.path != NULL)
		config.cwd.fd = open(config.cwd.path, O_RDONLY);

	if(config.cwd.fd < 0)
		goto_err(err_0, "retrieving working directory");

	/* parse config file */
	text = file_read(CONFIG_CONFIG_FILE, NULL);

	if(text == NULL){
		// having no config file is fine
		if(errno == ENOENT)
			return 0;

		goto_err(err_0, "reading config file \"%s\" failed", CONFIG_CONFIG_FILE);
	}

	jobj = json_tokener_parse(text);
	free((void*)text);

	if(jobj == NULL)
		goto_err(err_0, "parsing config file \"%s\" failed (json error)", CONFIG_CONFIG_FILE);

	config.log_file = json_object_get_string(json_object_object_get(jobj, "log-file"));
	config.verbose = json_object_get_boolean(json_object_object_get(jobj, "verbose"));

	jarr = json_object_object_get(jobj, "include-dirs");
	config.nincludes = json_object_array_length(jarr);

	config.include_dirs = calloc(1, sizeof(dir_t) * config.nincludes);

	if(config.include_dirs == NULL)
		goto err_1;

	for(size_t i=0; i<config.nincludes; i++){
		text = json_object_get_string(json_object_array_get_idx(jarr, i));
		config.include_dirs[i].path = realpath(text, NULL);

		if(config.include_dirs[i].path != NULL)
			config.include_dirs[i].fd = open(config.include_dirs[i].path, O_RDONLY);

		if(config.include_dirs[i].path == NULL || config.include_dirs[i].fd < 0)
			goto err_1;
	}

	json_object_put(jobj);

	return 0;


err_1:
	ERROR("initialising config failed");
	json_object_put(jobj);

err_0:
	config_free();

	return -1;
}

void config_free(){
	for(size_t i=0; i<config.nincludes; i++){
		close(config.include_dirs[i].fd);
		free((void*)config.include_dirs[i].path);
	}

	free(config.include_dirs);

	free((void*)config.cwd.path);
	close(config.cwd.fd);
}
