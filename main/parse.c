#include <string.h>
#include <config.h>
#include <symtab.h>
#include <log.h>


#define IFILE	"/home/jan/temp/test.dts"


int main(int argc, char **argv){
	int r;
	char *file;


	if(config_init() != 0)
		return 1;

	log_init(config.log_file);
	LOG("normal");
	VERBOSE("verbose");

	file = (argc > 1) ? argv[1] : IFILE;
	r = symtab_update(file, strlen(file), NULL, 0);

	if(r == 0)
		symtab_print();

	symtab_free();
	config_free();
	log_close();

	return r;
}
