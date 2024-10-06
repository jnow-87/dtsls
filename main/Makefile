#
# Copyright (C) 2024 Jan Nowotsch
# Author Jan Nowotsch	<jan.nowotsch@gmail.com>
#
# Released under the terms of the GNU GPL v2.0
#



bin-y := dtsls parse json-test


dtsls-y := \
	main.o \
	config.o \
	common/ \
	lsp/ \
	parser/ 

parse-y := \
	parse.o \
	config.o \
	common/ \
	parser/

dtsls-ldlibs := \
	-ljson-c

parse-ldlibs := \
	-ljson-c


json-test-y := json-test.o
json-test-ldlibs := -ljson-c
