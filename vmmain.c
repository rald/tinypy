#define COMPILER 0
#include "tp.c"
int main(int argc, char *argv[]) {
	tvm *tp = tinit(argc,argv);
	import(tp,argv[1],"__main__",0);
	deinit(tp);
	return(0);
}
