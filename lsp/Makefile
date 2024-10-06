#
# Copyright (C) 2024 Jan Nowotsch
# Author Jan Nowotsch	<jan.nowotsch@gmail.com>
#
# Released under the terms of the GNU GPL v2.0
#



obj-y := \
	cmds.o \
	complete.o \
	doc.o \
	lifecycle.o \
	lsp.o \
	symbol.o


$(loc_build_tree)/lsp.o: $(loc_build_tree)/cmds.hash.h

cmds-cflags-y := -Wno-old-style-definition
