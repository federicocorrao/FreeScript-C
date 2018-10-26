/*

*/   
    #include "object.h"
    #include "list.h"

/*

*/
    Object* NewObject(enum ObjectType t)
    {
        Object* o = (Object*) malloc(sizeof(Object));
        o -> Type = t;
        o -> O.AsList.Length = 0;       // clear union
        o -> O.AsList.First  = NULL;    // clear union
        o -> O.AsList.Last   = NULL;    // clear union
        return o;
    }
    
    void OBJ_Initialize()
    {
        OBJ_Undefined = NewObject(OBJTYPE_UNDEFINED);
        OBJ_Nil       = NewObject(OBJTYPE_NIL);
    }
    
    inline bool OBJ_IsUndefined (Object* o) { return o -> Type == OBJTYPE_UNDEFINED;    }
    inline bool OBJ_IsNil       (Object* o) { return o -> Type == OBJTYPE_NIL;          }
    inline bool OBJ_IsNumber    (Object* o) { return o -> Type == OBJTYPE_NUMBER;       }
    inline bool OBJ_IsIdentifier(Object* o) { return o -> Type == OBJTYPE_IDENTIFIER;   }    
    inline bool OBJ_IsReference (Object* o) { return o -> Type == OBJTYPE_REFERENCE;    }    
    inline bool OBJ_IsList      (Object* o) { return o -> Type == OBJTYPE_LIST;         }    
    inline bool OBJ_IsClosure   (Object* o) { return o -> Type == OBJTYPE_CLOSURE;      }    
    inline bool OBJ_IsBuiltin   (Object* o) { return o -> Type == OBJTYPE_BUILTIN;      }    
    inline bool OBJ_IsBaseType  (Object* o)
    {
        return  OBJ_IsUndefined(o)  ||
                OBJ_IsNil(o)        ||
                OBJ_IsNumber(o)     ||
                OBJ_IsClosure(o)    ;
    }

/*

*/
    Object* OBJ_Itself(Object* o)
    {
        if(o == NULL) return OBJ_Undefined;
        
        switch(o -> Type)
        {
            case OBJTYPE_UNDEFINED:
            case OBJTYPE_NIL:       
            case OBJTYPE_REFERENCE:
            case OBJTYPE_CLOSURE:
            case OBJTYPE_BUILTIN:
            case OBJTYPE_NUMBER:
                return o;
            case OBJTYPE_IDENTIFIER:
                return OBJ_Itself(o -> O.AsIdentifier.Value);
            case OBJTYPE_LIST:
            {
                if(OBJ_LIST_IsEmpty(o))
                    return OBJ_Undefined;
                else if(OBJ_LIST_IsSingleton(o))
                    return OBJ_Itself(o -> O.AsList.First -> Value);
                else
                    return o;
            }
        }
    }
    
/*

*/
    std::ostringstream _OBJ_StringStream;

    std::string OBJ_Serialize(Object* o)
    {
        _OBJ_RecursiveSerialize(o);
        std::string s = _OBJ_StringStream.str();
        _OBJ_StringStream.str("");  // reset
        _OBJ_StringStream.clear();  // reset
        return s;
    }
       
    void _OBJ_LE_Serialize(ListElement* le);
    
    void _OBJ_RecursiveSerialize(Object* o)
    {
        if(o == NULL)
            return;
        o = OBJ_Itself(o);
        
        switch(o -> Type)
        {
            case OBJTYPE_UNDEFINED: _OBJ_StringStream << "(undefined)"; return;
            case OBJTYPE_NIL:       _OBJ_StringStream << "(nil)";       return;
            case OBJTYPE_REFERENCE: _OBJ_StringStream << "(reference)"; return;
            case OBJTYPE_CLOSURE:   _OBJ_StringStream << "(closure)";   return;
            case OBJTYPE_BUILTIN:   _OBJ_StringStream << "(builtin)";   return;
            case OBJTYPE_NUMBER:
                _OBJ_StringStream   << "(number "
                                    << o -> O.AsNumber.Value
                                    << ")";
                                    return;
            case OBJTYPE_IDENTIFIER:
                _OBJ_StringStream   << "(identifier "
                                    << Bytecode_DecodeIdentifier(o -> O.AsIdentifier.Offset) 
                                    << ")";
                                    return;
            case OBJTYPE_LIST:
                _OBJ_StringStream   << "{list(" << (o -> O.AsList.Length) << "): ";
                _OBJ_LE_Serialize   (o -> O.AsList.First);
                _OBJ_StringStream   << "}";
                                    return;
        }
    }
    
        void _OBJ_LE_Serialize(ListElement* le)
        {
            if(le != NULL) {
                _OBJ_RecursiveSerialize(le -> Value);
                _OBJ_StringStream << ", ";
                _OBJ_LE_Serialize(le -> Next);
            }
        }

/*
    OK
*/    
    bool OBJ_True(Object* o)
    {
        if(o == NULL) return false;
    
        switch(o -> Type)
        {
            case OBJTYPE_UNDEFINED:
            case OBJTYPE_NIL:           return false;
            case OBJTYPE_CLOSURE:
            case OBJTYPE_BUILTIN:       return true;
            case OBJTYPE_NUMBER:        return o -> O.AsNumber.Value != 0;
            case OBJTYPE_IDENTIFIER:    return OBJ_True(o -> O.AsIdentifier.Value);
            case OBJTYPE_REFERENCE:     return OBJ_True(o -> O.AsReference.Pointer);
            case OBJTYPE_LIST:
            {
                ListElement* pointer = o -> O.AsList.First;
                bool istrue = true;
                while (pointer != NULL)
                {
                    istrue = istrue && OBJ_True(pointer -> Value);
                    if(istrue) break;
                    pointer = pointer -> Next;
                }
                return istrue;
            }
        }
    }

    inline bool OBJ_False(Object* o)
    {
        return !OBJ_True(o);
    }
    
/*

*/    
/*    Object* OBJ_Unserialize(char* o)
    {
        return NULL;
    }
*/

/*    Object* OBJ_Copy(Object* o)
    {
        if(o == NULL) return NULL;
        
        switch(o -> Type)
        {
            case OBJTYPE_UNDEFINED:
            case OBJTYPE_NIL:    
            case OBJTYPE_CLOSURE:
            case OBJTYPE_BUILTIN:
                return o;
            case OBJTYPE_NUMBER:
            case OBJTYPE_IDENTIFIER:
            case OBJTYPE_REFERENCE:
            case OBJTYPE_LIST:
                return o;
        }
    }
*/   








