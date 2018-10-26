%{
/* How this parser works.    
*/

    int yylex(void);                // C++ requires this
    void yyerror(const char *);     // C++ requires this

	Node* SyntaxTree;   // root of the tree
	int BytecodeSize;   // size of bytecode in machine words
	
%}
%define parse.error verbose

%union
{
    char* value;
    Node* node;
}

%token TERMINATOR UNKNOWN                               // no conflicts
%token BRACECLOSE BRACKCLOSE BRACEOPEN BRACKOPEN COMMA  // no conflicts
%token NIL UNDEFINED CLOSURE                            // no conflicts
%left  IF WHILE LOOP DO ASYNC PARENCLOSE                // all left
%right THEN ELSE                                        // dangling else
%right ASSIGN                                           // highest precedence
%left  PARENOPEN DOT                                    // highest precedence

%token <value> NUMBER
%token <value> IDENTIFIER
%type  <node>  Program Expression Sequence Value Params
%start Program

%%  /* Parser */

Program:
    Expression TERMINATOR                               /* Program */
    {
        $<node>$ = NewNode(NODETYPE_PROGRAM);               
        $<node>$ -> next1 = $<node>1;
        SyntaxTree = $<node>$;
        BytecodeSize += 1;
        // [PROGRAM]
        return 0;
    }
    | TERMINATOR                                        /* Empty Program */
    {
        $<node>$ = NewNode(NODETYPE_PROGRAM);
        SyntaxTree = $<node>$;
        BytecodeSize = 2;
        // [PROGRAM][NULL]
        return 0;
    }
;

Expression:
    BRACKOPEN Expression BRACKCLOSE                     /* Wrapped */
	{
	    $<node>$ = NewNode(NODETYPE_WRAPPED);
	    $<node>$ -> next1 = $<node>2;
	    BytecodeSize += 0;
	    // (skip)
	} 
    | IF Expression THEN Expression                     /* If-Then */
	{
	    $<node>$ = NewNode(NODETYPE_IF_THEN);
	    $<node>$ -> next1 = $<node>2;
	    $<node>$ -> next2 = $<node>4;
	    BytecodeSize += 2;
	    // [IF-THEN][I of Then-Expression]
	}
	| IF Expression THEN Expression ELSE Expression     /* If-Then-Else */
	{
	    $<node>$ = NewNode(NODETYPE_IF_THEN_ELSE);
	    $<node>$ -> next1 = $<node>2;
	    $<node>$ -> next2 = $<node>4;
	    $<node>$ -> next3 = $<node>6;
	    BytecodeSize += 3;
	    // [IF-THEN-ELSE][I of Then-Expression][I of Else-Expression]
	}
	| WHILE Expression LOOP Expression                  /* While-Loop */
	{
	    $<node>$ = NewNode(NODETYPE_WHILE_LOOP);
	    $<node>$ -> next1 = $<node>2;
	    $<node>$ -> next2 = $<node>4;
	    BytecodeSize += 2;
	    // [WHILE-LOOP][I of Loop-Expression]
	}
	| LOOP Expression WHILE Expression                  /* Loop-While */
	{
	    $<node>$ = NewNode(NODETYPE_LOOP_WHILE);
	    $<node>$ -> next1 = $<node>2;
	    $<node>$ -> next2 = $<node>4;
	    BytecodeSize += 2;
	    // [LOOP-WHILE][I of While-Expression]
	}
	| DO Expression                                     /* Do */
	{
	    $<node>$ = NewNode(NODETYPE_DO);
	    $<node>$ -> next1 = $<node>2;
	    BytecodeSize += 1;
	    // [DO]
	}
	| ASYNC Expression                                  /* Async */
	{
	    $<node>$ = NewNode(NODETYPE_ASYNC);
	    $<node>$ -> next1 = $<node>2;
	    BytecodeSize += 1;
	    // [ASYNC]
	}
	| Expression ASSIGN Expression                      /* Assign */
	{
	    $<node>$ = NewNode(NODETYPE_ASSIGN);
	    $<node>$ -> next1 = $<node>1;
	    $<node>$ -> next2 = $<node>3;
	    BytecodeSize += 2;
	    // [ASSIGN][I of Expression2]
	}
	| Expression PARENOPEN Sequence PARENCLOSE          /* Call */
	{
	    $<node>$ = NewNode(NODETYPE_CALL);
	    $<node>$ -> next1 = $<node>1;
	    $<node>$ -> next2 = $<node>3;
	    BytecodeSize += 2;
	    // [CALL][I of Sequence]
	}
	| Expression DOT IDENTIFIER                         /* Access */
	{
	    $<node>$ = NewNode(NODETYPE_ACCESS);
	    $<node>$ -> next1 = $<node>1;
	    $<node>$ -> value = (char*)
            malloc(strlen($<value>3) * sizeof(char));
        strcpy($<node>$ -> value, $<value>3);
        BytecodeSize += 2 + Bytecode_LengthInWords(strlen($<value>3));
   	    // [ACCESS][SIZE](string) 
	}
	| BRACEOPEN Sequence BRACECLOSE                     /* Block */
	{
	    $<node>$ = NewNode(NODETYPE_BLOCK);
	    $<node>$ -> next1 = $<node>2;
	    BytecodeSize += 1;
	    // [BLOCK]
	}
	| Value                                             /* Value */
	{
	    $<node>$ = NewNode(NODETYPE_VALUE);
	    $<node>$ -> next1 = $<node>1;
	    BytecodeSize += 0; 
	    // (skip)
	}
;

Sequence: 	
    Expression Sequence                                 /* Sequence */
    {
        $<node>$ = NewNode(NODETYPE_SEQUENCE);                  
        $<node>$ -> next1 = $<node>1;
        $<node>$ -> next2 = $<node>2;
        BytecodeSize += 2;
        // [SEQUENCE][I of Sequence]
    }
    | /* epsilon */                                     /* Sequence End */
    {
        $<node>$ = NULL;
        BytecodeSize += 1;
        // [NULL]
    }
;

Value:	
    IDENTIFIER                                          /* Identifier */
    {
        $<node>$ = NewNode(NODETYPE_IDENTIFIER);
        $<node>$ -> value = (char*)
            malloc(strlen($<value>1) * sizeof(char));
        strcpy($<node>$ -> value, $<value>1);
        BytecodeSize += 2 + Bytecode_LengthInWords(strlen($<value>1));
        // [IDENTIFIER][SIZE] (string)
    }
	| NUMBER                                            /* Number */
	{
	    $<node>$ = NewNode(NODETYPE_NUMBER);                    
        $<node>$ -> value = (char*)
            malloc(strlen($<value>1) * sizeof(char));
        strcpy($<node>$ -> value, $<value>1);
        BytecodeSize += 2;
        // [NUMBER] (number)
	}
	| NIL                                               /* Nil */
	{
	    $<node>$ = NewNode(NODETYPE_NIL);
	    BytecodeSize += 1;
	    // [NIL]
	}
	| UNDEFINED                                         /* Undefined */
	{
	    $<node>$ = NewNode(NODETYPE_UNDEFINED);                 
	    BytecodeSize += 1;
	    // [UNDEFINED]
	}
	| CLOSURE PARENOPEN Params PARENCLOSE Expression  /* Closure */
	{
	    $<node>$ = NewNode(NODETYPE_CLOSURE);
	    $<node>$ -> next1 = $<node>3;
	    $<node>$ -> next2 = $<node>5;
	    BytecodeSize += 2;
	    // [CLOSURE][I of Expression]
	}
;

Params:
    IDENTIFIER Params                                   /* Parameters */
    {
        $<node>$ = NewNode(NODETYPE_PARAMS);
        $<node>$ -> value = (char*)
            malloc(strlen($<value>1) * sizeof(char));
        strcpy($<node>$ -> value, $<value>1);
        $<node>$ -> next1 = $<node>2;
        BytecodeSize += 4 + Bytecode_LengthInWords(strlen($<value>1));
        // [PARAMS][Next][ID][SIZE] (string)
    }
    | /* epsilon */                                     /* Parameters End */
	{
	    $<node>$ = NULL;
        BytecodeSize += 1;
        // [NULL]
	}
;

%%

    #include "lexer.c"

    Node* Parse(char* input)
    {
        BytecodeSize = 0;
        SyntaxTree = NULL;
    
        yy_scan_string(input);
        if(!yyparse())
        {
            printf("> PARSER: OK\n");
            printf("> BYTECODE SIZE IS %d WORDS \n", BytecodeSize);        
        }
        yylex_destroy();
        return SyntaxTree;
    }

    void yyerror (const char *s)
    {
        fprintf(stderr, "> PARSER: %s\n", s);
    }

    void FreeTree(Node* tree)
    {
        return;
    }
    
    void PrintTree(Node* tree, char level)
    {
        int l;
        for(l = 0; l < level; ++l)
            printf("--|");
        
        if(tree == NULL)
            printf("-null-\n");
        else
        switch(tree -> type)
        {
            case NODETYPE_PROGRAM:
                printf("{program}\n");
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_WRAPPED:
                printf("{wrapped}\n");
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_IF_THEN:
                printf("{if-then}\n"); 
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            case NODETYPE_IF_THEN_ELSE:
                printf("{if-then-else}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                PrintTree(tree -> next3, level+1);
                break;
            case NODETYPE_WHILE_LOOP:
                printf("{while-loop}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            case NODETYPE_LOOP_WHILE:
                printf("{loop-while}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            case NODETYPE_DO:
                printf("{do}\n");
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_ASYNC:
                printf("{async}\n");
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_ASSIGN:
                printf("{assign}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            case NODETYPE_CALL:
                printf("{call}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            case NODETYPE_ACCESS:
                printf("{access: %s}\n", tree -> value);
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_BLOCK:
                printf("{block}\n");
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_SEQUENCE:
                printf("{sequence}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            case NODETYPE_VALUE:
                printf("{value}\n");
                PrintTree(tree -> next1, level+1);
                break;
            case NODETYPE_NUMBER:
                printf("(number %s)\n", tree -> value);
                break;
            case NODETYPE_IDENTIFIER:
                printf("(identifier %s)\n", tree -> value);
                break;
            case NODETYPE_NIL:
                printf("(nil)\n");
                break;
            case NODETYPE_UNDEFINED:
                printf("(undefined)\n");
                break;
            case NODETYPE_CLOSURE:
                printf("{closure}\n");
                PrintTree(tree -> next1, level+1);
                PrintTree(tree -> next2, level+1);
                break;
            default:
                break;
        }
    }

