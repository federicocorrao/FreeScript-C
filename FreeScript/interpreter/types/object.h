
#ifndef H_OBJECT
#define H_OBJECT

    struct STRUCT_OBJECT;
    typedef struct STRUCT_OBJECT Object;
    struct STRUCT_ENVIRONMENT;

    enum ObjectType
    {
        OBJTYPE_UNDEFINED,
        OBJTYPE_NIL,
        OBJTYPE_NUMBER,
        OBJTYPE_IDENTIFIER,
        OBJTYPE_REFERENCE,
        OBJTYPE_LIST,
        OBJTYPE_CLOSURE, 
        OBJTYPE_BUILTIN
    };

/* Public */

    Object* NewObject(enum ObjectType);         // Object constructor

    Object* OBJ_Undefined;                      // Global Undefined
    Object* OBJ_Nil;                            // Global Nil
        
    void        OBJ_Initialize  ();             // Initializing Nil and Undefined
    inline bool OBJ_IsUndefined (Object*);
    inline bool OBJ_IsNil       (Object*);
    inline bool OBJ_IsNumber    (Object*);
    inline bool OBJ_IsIdentifier(Object*);   
    inline bool OBJ_IsReference (Object*);    
    inline bool OBJ_IsList      (Object*);    
    inline bool OBJ_IsClosure   (Object*);   
    inline bool OBJ_IsBuiltin   (Object*);   
    inline bool OBJ_IsBaseType  (Object*);
    
    Object*     OBJ_Itself      (Object*);
    Object*     OBJ_Copy        (Object*);
    std::string OBJ_Serialize   (Object*);      
    Object*     OBJ_Unserialize (char*);
    bool        OBJ_True        (Object*);
    
/* Private */    
    
    void _OBJ_RecursiveSerialize(Object* o);

/* */

    struct STRUCT_NUMBER
    {
        float Value;
    };

    struct STRUCT_IDENTIFIER
    {
        int* Offset;
        struct STRUCT_OBJECT* Value;
    };
    
        struct STRUCT_LIST_ELEMENT
        {
            struct STRUCT_LIST_ELEMENT *Next;
            struct STRUCT_LIST_ELEMENT *Prev;
            struct STRUCT_OBJECT       *Value;
        };
        typedef struct STRUCT_LIST_ELEMENT ListElement;
        
    struct STRUCT_LIST
    {
        int Length;
        ListElement* First;
        ListElement* Last;
    };
    
    struct STRUCT_REFERENCE
    {
        struct STRUCT_OBJECT *Pointer;    
    };
    
    struct STRUCT_CLOSURE
    {
        int Offset;
        int** Params; // vettore di offsets
        struct STRUCT_ENVIRONMENT* Environment;
    };
    
    struct STRUCT_BUILTIN
    {
        int* Name; // encoded name in heap, not bytecode
        Object* (*NativePointer)(Object*);
    };   
    
    struct STRUCT_OBJECT
    {
        enum ObjectType Type;
                        
        union UNION_OBJECT
        {
            struct STRUCT_NUMBER        AsNumber;
            struct STRUCT_IDENTIFIER    AsIdentifier;
            struct STRUCT_LIST          AsList;
            struct STRUCT_REFERENCE     AsReference;
            struct STRUCT_CLOSURE       AsClosure;
            struct STRUCT_BUILTIN       AsBuiltin;
        } O;
    };
    
#endif

