/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef CONFIG_H
#define CONFIG_H


#include <stdbool.h>
#include <sys/types.h>


/* types */
typedef struct{
	char const *path;
	int fd;
} dir_t;

typedef struct{
	dir_t cwd;
	dir_t *include_dirs;
	size_t nincludes;

	char const *log_file;
	bool verbose;
} config_t;


/* external variables */
extern config_t config;


/* prototypes */
int config_init();
void config_free();


#endif // CONFIG_H
