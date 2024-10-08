/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <sys/types.h>


/* prototypes */
char *stralloc(char const *s, size_t n);
int strmatch(char const *s, FILE *fp);
char *file_read(char const *path, size_t *size);
int file_locate_identifier(char const *text, size_t line, size_t column, char *idfr, size_t idfr_max, bool trunc);


#endif // UTILS_H
