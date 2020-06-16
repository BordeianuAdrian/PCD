#include </usr/include/python2.7/Python.h>
#include <stdio.h>

void main22();

int main(int argc, char* argv[]){


	// FILE* fp;

	// Py_Initialize();

	// fp = fopen(filename, "r");
	// PyRun_SimpleFile(fp, filename);
	// printf("finish");

	// Py_Finalize();
	main22();
}

void main22(){
char filename[] = "p.py";
    FILE* file;
    int argc;
    char * argv[3];

    argc = 3;
    argv[0] = "p.py";
    argv[1] = "muie fortis";
    argv[2] = "rotateImg";

    Py_SetProgramName(argv[0]);
    Py_Initialize();
    PySys_SetArgv(argc, argv);
	file = fopen(filename, "r");
    PyRun_SimpleFile(file, "p.py");
    Py_Finalize();

}