#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){


	system("gcc -o scanner scanner.c && ./scanner");
	system("gcc -o parser parser.c && ./parser");
	system("gcc -o vm vm.c && ./vm");
	return 0;
}