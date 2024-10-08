/**
 * Copyright (C) 2024 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>


/* local/static prototypes */
static int iskeyword(char c);


/* global functions */
char *stralloc(char const *s, size_t n){
	char *x;


	x = malloc(n + 1);

	if(x == NULL)
		return NULL;

	memcpy(x, s, n);
	x[n] = 0;

	return x;
}

int strmatch(char const *s, FILE *fp){
	size_t len = strlen(s);
	char c;


	for(size_t i=0; i<len; i++){
		if(fread(&c, 1, 1, fp) != 1)
			return -1; // stream error

		if(c != s[i])
			return 1; // indicate retry
	}

	return 0; // success
}

char *file_read(char const *path, size_t *size){
	char *text;
	FILE *fp;
	struct stat st;


	if(stat(path, &st) != 0)
		return NULL;

	fp = fopen(path, "r");

	if(fp == NULL)
		return NULL;

	text = malloc(st.st_size + 1);

	if(text != NULL){
		if(size != NULL)
			*size = st.st_size;

		fread(text, st.st_size, 1, fp);
		text[st.st_size] = 0;
	}

	fclose(fp);

	return text;
}

int file_locate_identifier(char const *text, size_t line, size_t column, char *idfr, size_t idfr_max, bool trunc){
	size_t i = 0;
	size_t start;


	/* move to line */
	while(line > 1 && text[i]!=0){
		if(text[i++] == '\n')
			line--;
	}

	/* move to column */
	while(column > 1 && text[i]!=0){
		if(text[i++] == '\n')
			return -1;

		column--;
	}

	if(!iskeyword(text[i])){
		if(i == 0 || !iskeyword(text[i - 1]))
			return -1;

		i--;
	}

	/* find start */
	for(start=i; start>0; start--){
		if(!iskeyword(text[start]))
			break;
	}

	if(!iskeyword(text[start]))
		start++;

	/* find end */
	if(!trunc){
		while(iskeyword(text[i]))
			i++;
	}

	/* copy text */
	i = MIN(idfr_max - 1, i - start);
	strncpy(idfr, text + start, i);
	idfr[i] = 0;

	return 0;
}


/* local functions */
static int iskeyword(char c){
	return (isalnum(c) || c == '_');
}
