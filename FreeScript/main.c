
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <string>   // C++ string
#include <iostream> // C++ cout
#include <sstream>  // C++ string stream

#include "frontend/commons.h"
#include "frontend/parser.c"
#include "frontend/bytecode.c"
#include "interpreter/vm.c"

     
    int FileSize(FILE* fp)
    {
        fseek(fp, 0, SEEK_END); 
        int size = ftell(fp);
        fseek(fp, 0, SEEK_SET); 
        return size;
    }

    char* ReadFile(const char* filename)
    {
        FILE* fp = fopen(filename, "r");
        int size = FileSize(fp);
        char* buffer = (char*) malloc(size * sizeof(char));
        fread(buffer, size, sizeof(char), fp);
        return buffer;
    }

/* */

    int main(int argc, char** argv)
    {    
        char* input = ReadFile("input.free");
        Node* tree = Parse(input);
        system("sleep 1");
        // PrintTree(tree, 0);
        int* program = Bytecode_Compile(tree, BytecodeSize);
        
        int i; for(i = 0; i < BytecodeSize; i++)
            printf("%4d: %d\n", i, program[i]);
        
        OBJ_Initialize();
        Object* result = VM_Run(program);
        
        return 0;
    }
	
	 
	 
	 
