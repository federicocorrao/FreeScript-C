/*
    Fontend and Bytecode Commons
*/

    inline int  Bytecode_LengthInWords(int length);

    enum NodeType
    {
        NODETYPE_NULL           = 0,
        NODETYPE_PROGRAM        = 1,
        NODETYPE_WRAPPED        = 2, // Never in Bytecode
        NODETYPE_IF_THEN        = 3,
        NODETYPE_IF_THEN_ELSE   = 4,
        NODETYPE_WHILE_LOOP     = 5,
        NODETYPE_LOOP_WHILE     = 6,
        NODETYPE_DO             = 7,
        NODETYPE_ASYNC          = 8,
        
        NODETYPE_ASSIGN         = 9,
        NODETYPE_CALL           = 10,
        NODETYPE_ACCESS         = 11,
        
        NODETYPE_BLOCK          = 12,
        NODETYPE_SEQUENCE       = 13,
        NODETYPE_VALUE          = 14, // Never in Bytecode
        
        NODETYPE_NUMBER         = 15,
        NODETYPE_IDENTIFIER     = 16,
        NODETYPE_NIL            = 17,
        NODETYPE_UNDEFINED      = 18,
        NODETYPE_CLOSURE        = 19,
        NODETYPE_PARAMS         = 20
    };    
    
    struct STRUCT_NODE
    {
        enum NodeType type;
        struct STRUCT_NODE *next1;
        struct STRUCT_NODE *next2;
        struct STRUCT_NODE *next3;
        char* value;  
    };
    typedef struct STRUCT_NODE Node;
	
	Node* NewNode(enum NodeType t)
	{	
		Node* n = (Node*) malloc(sizeof(Node));
		n -> type  = t;
		n -> next1 = NULL;
		n -> next2 = NULL;
		n -> next3 = NULL;
		n -> value = NULL;
		return n;
	}
	
	/* Length of an identifier in words*/
    inline int Bytecode_LengthInWords(int length)
    {
        return 1 + (int)(((float)length - 1) / sizeof(int));
    }
	
/* */	
	
	
	

