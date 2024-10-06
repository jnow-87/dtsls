/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <config.h>
#include <log.h>


/* static variables */
int log_fd = 2;


/* global functions */
void log_init(char const *file){
	if(file == NULL)
		return;

	log_fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if(log_fd >= 0)
		return;

	log_fd = 2;
	ERROR("opening log-file \"%s\", falling back to stderr", file);
}

void log_close(){
	if(log_fd != 2)
		close(log_fd);
}

int log_print(log_lvl_t lvl, char const *fmt, ...){
	va_list lst;


	if(lvl == LOG_VERBOSE && !config.verbose)
		return 0;

	if(lvl == LOG_ERROR)
		dprintf(log_fd, "error: ");

	va_start(lst, fmt);
	vdprintf(log_fd, fmt, lst);
	va_end(lst);

	if(lvl == LOG_ERROR && errno)
		dprintf(log_fd, ": %s", strerror(errno));

	dprintf(log_fd, "\n");

	return -(lvl == LOG_ERROR);
}
