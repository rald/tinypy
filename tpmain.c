#include "tp.c"
int main(int argc, char *argv[]) {
	tvm *tp = tinit(argc,argv);
	call(tp,"py2bc","tinypy",None);
	deinit(tp);
	return(0);
}
