/* 
    Freescript Bytecode Specification

    {null}          [ | | | NUL ]
    (nil)           [ | | | NIL ]
    (undefined)     [ | | | UND ]
    
    {program}       [ | | | PRG ] {exp}
    {do}            [ | | | DO  ] {exp}
    {async}         [ | | | ASY ] {exp}

    {if-then}       [ | | | IF  ] [exp2 index] {exp1} {exp2} 
    {while-loop}    [ | | | WH  ] [exp2 index] {exp1} {exp2}  
    {loop-while}    [ | | | LO  ] [exp2 index] {exp1} {exp2}
    {assign}        [ | | | ASN ] [exp2 index] {exp1} {exp2}
    {closure}       [ | | | CLS ] [exp  index] {params} {exp }
    {call}          [ | | | CLL ] [seq  index] {exp } {seq }
    
    {if-then-else}  [ | | | IFE ] [exp2 index] [exp3 index] {exp1} {exp2} {exp3}   
    
    {block}         [ | | do | BLOCK] {exp}
    {sequence}      [ | | do | SEQ  ] [seq index] {exp} {seq}
    {value}         skip
    {wrapped}       skip

    (number)        [ sign | floa | size | NUM ] [] ... []
    (identifier)    [      |      | size | ID  ] [i d e n] ... [t i 0 0]
    {access}        [      |      | size | ACC ] [i d e n] ... [t i 0 0] {exp}

FOR NOW:

    (number)        [ | | | NUM  ] {number}
    (identifier)    [ | | | ID   ] [size] [i d e n] ... [t i 0 0]
    {access}        [ | | | ACC  ] [size] [i d e n] ... [t i 0 0]
    {params}        [ | | | PARS ] [size] [i d e n] ... [t i 0 0] {params}

*/

/*  Recursive Compiler:
    Linearizes the abstract syntax tree into a sequence of bytes.
    Follows the previous specification.
    The sequence contains integer pointers to itself.
    DO NOT TOUCH
*/    

/* Public */

    int*        Bytecode_Compile            (Node* tree, int bufferSize);
    char*       Bytecode_DecodeIdentifier   (int offset);
    int*        Bytecode_EncodeIdentifier   (char* string);
    inline char Bytecode_GetKthChar         (int k, int word);

/* Private */

    int*  _BC_Buffer;
    int   _BC_Offset;
    int   _BC_RecursiveCompile(Node*);


/* Bytecode_Compile
*/    
    int* Bytecode_Compile(Node* tree, int bufferSize)
    {
        _BC_Buffer = (int*) calloc(bufferSize, sizeof(int));
        _BC_Offset = 0;
        _BC_RecursiveCompile(tree);    
        return _BC_Buffer;
    }


/* _BC_RecursiveCompile
*/
    int _BC_RecursiveCompile(Node* tree)
    {
        int offset = _BC_Offset;

        if(tree == NULL)
        {
            _BC_Buffer[offset] = NODETYPE_NULL;
            _BC_Offset += 1;
        }
        else
        switch(tree -> type)
        {
            case NODETYPE_NIL:
            case NODETYPE_UNDEFINED:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 1;
                break;
            }   
            case NODETYPE_PROGRAM:
            case NODETYPE_DO:
            case NODETYPE_ASYNC:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 1;
                _BC_RecursiveCompile(tree -> next1);
                break;
            }    
            case NODETYPE_IF_THEN:
            case NODETYPE_WHILE_LOOP:
            case NODETYPE_LOOP_WHILE:
            case NODETYPE_ASSIGN:
            case NODETYPE_CALL:
            case NODETYPE_SEQUENCE:
            case NODETYPE_CLOSURE:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 2;
                _BC_RecursiveCompile(tree -> next1);
                _BC_Buffer[offset + 1] = _BC_RecursiveCompile(tree -> next2);
                break;
            }            
            case NODETYPE_IF_THEN_ELSE:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 3;
                _BC_RecursiveCompile(tree -> next1);
                _BC_Buffer[offset + 1] = _BC_RecursiveCompile(tree -> next2);
                _BC_Buffer[offset + 2] = _BC_RecursiveCompile(tree -> next3);
                break;
            }
            case NODETYPE_BLOCK:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 1;
                _BC_RecursiveCompile(tree -> next1);
                break;
            }
            case NODETYPE_VALUE:
            case NODETYPE_WRAPPED:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 0;
                _BC_RecursiveCompile(tree -> next1);
                break;
            }    
            case NODETYPE_NUMBER:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 2;
                float num_f = atof(tree -> value);
                int num_i;
                memcpy(&num_i, &num_f, sizeof(int));
                _BC_Buffer[offset + 1] = num_i;
                break;
            }
            case NODETYPE_IDENTIFIER:
            {
                int len = strlen(tree -> value);
                int wordlen = Bytecode_LengthInWords(len);              // length in machine words  
                int* b = Bytecode_EncodeIdentifier(tree -> value);
                
                memcpy( _BC_Buffer + offset,
                        b,
                        (wordlen + 2) * sizeof(int)
                    );
                
                _BC_Offset += 2 + wordlen;                              // update offset
                break;
            }
            case NODETYPE_ACCESS:
            {
                int len = strlen(tree -> value);
                int wordlen = Bytecode_LengthInWords(len);              // length in machine words  
                int* b = Bytecode_EncodeIdentifier(tree -> value);
                
                memcpy( (void*)(_BC_Buffer + offset),
                        (void*)b,
                        (wordlen +2)*sizeof(int)
                    );
                _BC_Offset += 2 + wordlen;                              // update offset
                _BC_RecursiveCompile(tree -> next1);                    // compile expression
                break;
            }
            case NODETYPE_PARAMS:
            {
                _BC_Buffer[offset] = tree -> type;
                _BC_Offset += 2;
                
                int len = strlen(tree -> value);
                int wordlen = Bytecode_LengthInWords(len);              // length in machine words  
                int* b = Bytecode_EncodeIdentifier(tree -> value);
                
                memcpy( _BC_Buffer + offset + 2,
                        b,
                        (wordlen + 2) * sizeof(int)
                    );
                
                _BC_Offset += 2 + wordlen;                              // update offset
                _BC_Buffer[offset + 1] = _BC_RecursiveCompile(tree -> next1);
                break;
            } 
            default:
            {
                printf("> BYTECODE COMPILER: Unrecognized Type\n");
            }
        }
        return offset;
    }


/* 
*/
    inline char Bytecode_GetKthChar(int k, int word)
    {
        return (char) (word >> (k * 8)) & 255;
    }

/*
*/
    int* Bytecode_EncodeIdentifier(char* string)
    {
        int len = strlen(string);
        int wordlen = Bytecode_LengthInWords(len);                  // length in machine words  
        int* buffer = (int*) malloc((2 + wordlen) * sizeof(int));
        buffer[0]   = NODETYPE_IDENTIFIER;
        buffer[1]   = wordlen;  

        int w; for(w = 0; w < wordlen; w++)                         // foreach word
        {
            int word = 0;                                           // clear word
            int c; for(c = 0; c < sizeof(int); c++)                 // foreach char in word
            {
                int charpos = w * sizeof(int) + c;                  // char position in identifier
                char character = (charpos < len) ?                  // get character
                    (char)(string[charpos]) : 0;                    // (or fill with zero)
                word |= character << (8 * c);                       // append char to word
                
            }
            buffer[2 + w] = word;                                    // append word to code
        }  
        return buffer;
    }

/*  Decodes an identifier name from the bytecode.
    Here index is the position of [IDENTIFIER] in bytecode.
*/
    char* Bytecode_DecodeIdentifier(int* offset)
    {
        int wordlen = *(offset + 1);
       
        int len = wordlen * sizeof(int) + 1;                            // + 1 for \0
        char* destination = (char*) calloc(len, sizeof(char));          // new string
        
        int di = 0;
        int w; for(w = 0; w < wordlen; w++)                             // foreach word
        {
            int word = *(offset + 2 + w);                               // getting word
            
            int c; for(c = 0; c < sizeof(int); c++, di++)               // foreach char in word
            {
                char character = Bytecode_GetKthChar(c, word);          // getting c-th char
                destination[di] = character;                            // appending char
            }
        }
        return destination;
    }
