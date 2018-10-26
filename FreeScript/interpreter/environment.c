
    struct STRUCT_BINDING
    {
        struct STRUCT_BINDING *Next;
        Object                *Identifier;
    };
    typedef struct STRUCT_BINDING Binding;
    
    struct STRUCT_ENVIRONMENT
    {
        struct STRUCT_ENVIRONMENT *Parent;
        Binding *Bindings;
    };
    typedef struct STRUCT_ENVIRONMENT Environment;

/*

*/    
    Environment* NewEnvironment()
    {
        Environment* e = (Environment*) malloc(sizeof(Environment));
        e -> Bindings = NULL;
        e -> Parent = NULL;
        return e;
    }    

    Binding* NewBinding()
    {
        Binding* b = (Binding*) malloc(sizeof(Binding));
        return b;
    }

/* 
    Compare identifiers in bytecode
*/

    int _ENV_Compare(int* o1, int* o2) 
    {
        return memcmp(
            o1,
            o2,
            (*(o1 + 1) + 2) * sizeof(int)
        );
    }

/*

*/
    Object* ENV_Get(Environment* e, int* offset)
    {
        Binding* pointer = e -> Bindings;
        while(pointer)
        {
            if (_ENV_Compare(
                    pointer -> Identifier -> O.AsIdentifier.Offset,
                    offset) == 0
               )
                return pointer -> Identifier;
            else
                pointer = pointer -> Next;
        }
        // std::cout << "ENV_Get: " << Bytecode_DecodeIdentifier(offset) <<
        //    " NOT IN CURRENT ENVIRONMENT\n";
        
        if(e -> Parent)
            return ENV_Get(e -> Parent, offset);
        else 
        {
            // Binding* b = NewBinding();
            Object* id = NewObject(OBJTYPE_IDENTIFIER);
            id -> O.AsIdentifier.Offset = offset;
            id -> O.AsIdentifier.Value  = OBJ_Undefined;
            // b -> Identifier = id;
            // b -> Next  = e -> Bindings;
            // e -> Bindings = b;
            return id;
        }
    }

/*

*/   
    void ENV_Set(Environment* e, Object* identifier)
    {
        Binding* pointer = e -> Bindings;
        while(pointer)
        {
            if (_ENV_Compare(
                    pointer -> Identifier -> O.AsIdentifier.Offset,
                    identifier -> O.AsIdentifier.Offset) == 0
                )
            {
                pointer -> Identifier = identifier;                             
                return;
            }
            else
                pointer = pointer -> Next;
        }
        // Not found: add new binding
        Binding* b = NewBinding();
        b -> Identifier = identifier;
        b -> Next  = e -> Bindings;
        e -> Bindings = b;
    }

/*

*/
    void ENV_Set_Builtin(Environment* e, Object* builtin)
    {
        Object* id = NewObject(OBJTYPE_IDENTIFIER);
        id -> O.AsIdentifier.Offset = builtin -> O.AsBuiltin.Name;
        id -> O.AsIdentifier.Value = builtin;
        ENV_Set(e, id);
    }




