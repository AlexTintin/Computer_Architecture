

#include "world.h"

using namespace std;



void hello_world(int argc, char *argv[]) {
	printf("\n\nHello World!\n\n");

	for (unsigned int i = 0; i < (unsigned int)argc; i++)
	   printf("argument #%d = %s\n", i, argv[i]);

	printf("\n\n");
}
