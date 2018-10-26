/*  

*/
#include "types/object.c"
#include "types/list.c"
#include "environment.c"

#include <time.h>

using namespace std;

/* Public */
    
    void    Initialize();
    Object* VM_Run(int* expression);

/* Private */

    int*            _VM_Program;
    Object*         _VM_Evaluate            (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Null       (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Program    (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_IfThen     (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_IfThenElse (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_WhileLoop  (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_LoopWhile  (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Do         (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Async      (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Assign     (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Call       (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Access     (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Block      (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Sequence   (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Number     (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Identifier (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Nil        (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Undefined  (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Closure    (unsigned int offset, Environment* env);
    inline Object*  _VM_Evaluate_Params     (unsigned int offset, Environment* env);
    
/*
*/
    
    Object* builtin_sum(Object* params)
    {
        Object* p1 = OBJ_LIST_At(params, 0);
        Object* p2 = OBJ_LIST_At(params, 1);
        
        Object* r = NewObject(OBJTYPE_NUMBER);
        r -> O.AsNumber.Value =
            p1 -> O.AsNumber.Value - p2 -> O.AsNumber.Value;

        return r;
    }
    
    Object* builtin_echo(Object* params)
    {
        int i; for (i = 0; i < params -> O.AsList.Length; i++)
            cout << "ECHO: " << OBJ_Serialize(OBJ_LIST_At(params, i)) << "\n"; 
        return OBJ_Undefined;
    }
    
    Environment* _VM_MainEnv;
    
    void Initialize()
    {
        _VM_MainEnv = NewEnvironment();
        
        Object* plus = NewObject(OBJTYPE_BUILTIN);
        int* name = Bytecode_EncodeIdentifier((char*)"-");
        plus -> O.AsBuiltin.Name = name;
        plus -> O.AsBuiltin.NativePointer = &builtin_sum;
        ENV_Set_Builtin(_VM_MainEnv, plus);
        
        Object* echo = NewObject(OBJTYPE_BUILTIN);
        int* namee = Bytecode_EncodeIdentifier((char*)"echo");
        echo -> O.AsBuiltin.Name = namee;
        echo -> O.AsBuiltin.NativePointer = &builtin_echo;
        ENV_Set_Builtin(_VM_MainEnv, echo);
    }

    Object* VM_Run(int* expression)
    {
        Initialize();
        _VM_Program = expression;
        
        clock_t start = clock();  
        Object* result = _VM_Evaluate(0, _VM_MainEnv);
        clock_t end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;  
        
        cout << "> VM: DONE in " << seconds << "\n";
        cout << "> VM: " << OBJ_Serialize(result) << "\n";
        return result;
    }

/*
*/
    Object* _VM_Evaluate(unsigned int offset, Environment* env)
    {
        switch(_VM_Program[offset])
        {
            case NODETYPE_NULL:         return _VM_Evaluate_Null       (offset, env);
            case NODETYPE_PROGRAM:      return _VM_Evaluate_Program    (offset, env);
            case NODETYPE_IF_THEN:      return _VM_Evaluate_IfThen     (offset, env);
            case NODETYPE_IF_THEN_ELSE: return _VM_Evaluate_IfThenElse (offset, env);
            case NODETYPE_WHILE_LOOP:   return _VM_Evaluate_WhileLoop  (offset, env);
            case NODETYPE_LOOP_WHILE:   return _VM_Evaluate_LoopWhile  (offset, env);
            case NODETYPE_DO:           return _VM_Evaluate_Do         (offset, env);
            case NODETYPE_ASYNC:        return _VM_Evaluate_Async      (offset, env);
            case NODETYPE_ASSIGN:       return _VM_Evaluate_Assign     (offset, env);
            case NODETYPE_CALL:         return _VM_Evaluate_Call       (offset, env);
            case NODETYPE_ACCESS:       return _VM_Evaluate_Access     (offset, env);
            case NODETYPE_BLOCK:        return _VM_Evaluate_Block      (offset, env);
            case NODETYPE_SEQUENCE:     return _VM_Evaluate_Sequence   (offset, env);
            case NODETYPE_NUMBER:       return _VM_Evaluate_Number     (offset, env);
            case NODETYPE_IDENTIFIER:   return _VM_Evaluate_Identifier (offset, env);
            case NODETYPE_NIL:          return _VM_Evaluate_Nil        (offset, env);
            case NODETYPE_UNDEFINED:    return _VM_Evaluate_Undefined  (offset, env);
            case NODETYPE_CLOSURE:      return _VM_Evaluate_Closure    (offset, env);
            case NODETYPE_PARAMS:       return _VM_Evaluate_Params     (offset, env);
            default:
                printf("opcode Not recognized");
        }
    }

/**/

    inline Object* _VM_Evaluate_Null(unsigned int offset, Environment* env)     // OK
    {
        return OBJ_Undefined;
    }
    
    inline Object* _VM_Evaluate_Program(unsigned int offset, Environment* env)  // OK
    {
        return _VM_Evaluate(offset + 1, env);
    }
    
    inline Object* _VM_Evaluate_IfThen(unsigned int offset, Environment* env)   // OK
    {
        Object* guard = _VM_Evaluate(offset + 2, env);
        if(OBJ_True(guard))
            return _VM_Evaluate(_VM_Program[offset + 1], env);
        else
            return OBJ_Undefined;
    }
   
    inline Object* _VM_Evaluate_IfThenElse(unsigned int offset, Environment* env) // OK
    {
        Object* guard = _VM_Evaluate(offset + 3, env);
        if(OBJ_True(guard))
            return _VM_Evaluate(_VM_Program[offset + 1], env);
        else
            return _VM_Evaluate(_VM_Program[offset + 2], env);
    }
    
    inline Object* _VM_Evaluate_WhileLoop(unsigned int offset, Environment* env) // OK
    {
        Object* list = NewObject(OBJTYPE_LIST);
        
        while(OBJ_True(_VM_Evaluate(offset + 2, env)))
           OBJ_LIST_AppendElement(list,
                _VM_Evaluate(_VM_Program[offset + 1], env));
        return list;
    }
    
    inline Object* _VM_Evaluate_LoopWhile(unsigned int offset, Environment* env) // OK
    {
        Object* list = NewObject(OBJTYPE_LIST);
        
        do OBJ_LIST_AppendElement(list,
                _VM_Evaluate(_VM_Program[offset + 1], env));
        while(OBJ_True(_VM_Evaluate(offset + 2, env)));
        return list;
    }
    
    inline Object* _VM_Evaluate_Do(unsigned int offset, Environment* env)       // OK
    {
        _VM_Evaluate(offset + 1, env);
        return OBJ_Undefined;
    }
    
    inline Object* _VM_Evaluate_Async(unsigned int offset, Environment* env)
    {
        return OBJ_Undefined;
    }
    
    inline Object* _VM_Evaluate_Assign(unsigned int offset, Environment* env)   // OK
    {
        Object* target = _VM_Evaluate(offset + 2, env);
        Object* value  = _VM_Evaluate(_VM_Program[offset + 1], env);
        
        if(OBJ_IsIdentifier(target))
        {
            target -> O.AsIdentifier.Value = OBJ_Itself(value); // copy?
            ENV_Set(env, target);
        }
        else
            cout << "WARNING: Not an identifier\n";
        
        return value;
    }
    
    inline Object* _VM_Evaluate_Call(unsigned int offset, Environment* env) // OK
    {
        Object* paramlist = _VM_Evaluate_Sequence(_VM_Program[offset + 1], env);
        Object* callee    = _VM_Evaluate(offset + 2, env);
        
        if(OBJ_IsIdentifier(callee))
        {
            if(OBJ_IsBuiltin(callee -> O.AsIdentifier.Value))
            {
                return (callee -> O.AsIdentifier.Value -> O.AsBuiltin.NativePointer)(paramlist);
            }
            else if(OBJ_IsClosure(callee -> O.AsIdentifier.Value))
            {
                Object* closure = callee -> O.AsIdentifier.Value;
                ListElement* param_value = paramlist -> O.AsList.First;
                
                int i = 0;
                for(i = 0; i < paramlist -> O.AsList.Length; i++)
                {
                    Object* id = NewObject(OBJTYPE_IDENTIFIER);
                    id -> O.AsIdentifier.Offset = closure -> O.AsClosure.Params[i];
                    id -> O.AsIdentifier.Value = OBJ_LIST_At(paramlist, i);     // FIX ottimizzare 
                    ENV_Set(closure -> O.AsClosure.Environment, id);
                }                         
                return _VM_Evaluate(closure -> O.AsClosure.Offset,
                    closure -> O.AsClosure.Environment);
            }
            else
                cout << "call: not found\n";
        }
        return OBJ_Undefined;
    }
    
    inline Object* _VM_Evaluate_Access(unsigned int offset, Environment* env)
    {
        return OBJ_Undefined;
    }
    
    inline Object* _VM_Evaluate_Block(unsigned int offset, Environment* env)    // OK
    {
        return _VM_Evaluate_Sequence(offset + 1, env);
    }
    
    inline Object* _VM_Evaluate_Sequence(unsigned int offset, Environment* env) // OK 
    {   
        Object* list = NewObject(OBJTYPE_LIST);
           
        int next = offset;
        while(_VM_Program[next])
        {
            Object* element = _VM_Evaluate(next + 2, env);
       
            OBJ_LIST_AppendElement(
                list,
                OBJ_Itself(element)
            );   
            next = _VM_Program[next + 1];
        }
        return list;
    }
    
    inline Object* _VM_Evaluate_Number(unsigned int offset, Environment* env)   // OK 
    {
        Object* number = NewObject(OBJTYPE_NUMBER);
        
        memcpy( &(number -> O.AsNumber.Value),                                  // Assuming sizeof(int) == sizeof(float)    
                &(_VM_Program[offset + 1]),
                sizeof(float));
        return number;
    }
    
    inline Object* _VM_Evaluate_Identifier(unsigned int offset, Environment* env) // OK 
    {
        return ENV_Get(env, _VM_Program + offset);
    }
    
    inline Object* _VM_Evaluate_Nil(unsigned int offset, Environment* env)      // OK 
    {
        return OBJ_Nil;
    }
    
    inline Object* _VM_Evaluate_Undefined(unsigned int offset, Environment* env)  // OK
    {
        return OBJ_Undefined;
    }
    
    inline Object* _VM_Evaluate_Closure(unsigned int offset, Environment* env)
    {
        Environment* e = NewEnvironment();
        e -> Parent = env;
        // Fetching Parameters
        // TODO: in bytecode numero di parametri (prepend)
        int** params = (int**) calloc(10, sizeof(int*));
        int paramoffset = offset + 2;
        int i = 0;
        while(_VM_Program[paramoffset]) // != 0
        {
            params[i] = &(_VM_Program[paramoffset + 2]); // indirizzo identificatore
            paramoffset = _VM_Program[paramoffset + 1];
            i++;
        }
        // Creating Closure      
        Object* closure = NewObject(OBJTYPE_CLOSURE);
        closure -> O.AsClosure.Environment = e;
        closure -> O.AsClosure.Offset = _VM_Program[offset + 1]; // body position
        closure -> O.AsClosure.Params = params;     
        return closure;
    }
    
    inline Object* _VM_Evaluate_Params(unsigned int offset, Environment* env)
    {
        cout << "param " << Bytecode_DecodeIdentifier(&_VM_Program[offset+2]) << "\n";
        return OBJ_Undefined;
    }
    
    
/* */   
/*
static void* ops[] =
{ 
    &&L_NODETYPE_NULL           ,//= 0,
    &&L_NODETYPE_PROGRAM        ,//= 1,
    &&L_NODETYPE_WRAPPED        ,//= 2, // Never in Bytecode
    &&L_NODETYPE_IF_THEN        ,//= 3,
    &&L_NODETYPE_IF_THEN_ELSE   ,//= 4,
    &&L_NODETYPE_WHILE_LOOP     ,//= 5,
    &&L_NODETYPE_LOOP_WHILE     ,//= 6,
    &&L_NODETYPE_DO             ,//= 7,
    &&L_NODETYPE_ASYNC          ,//= 8,      
    &&L_NODETYPE_ASSIGN         ,//= 9,
    &&L_NODETYPE_CALL           ,//= 10,
    &&L_NODETYPE_ACCESS         ,//= 11,
    &&L_NODETYPE_BLOCK          ,//= 12,
    &&L_NODETYPE_SEQUENCE       ,//= 13,
    &&L_NODETYPE_VALUE          ,//= 14, // Never in Bytecode
    &&L_NODETYPE_NUMBER         ,//= 15,
    &&L_NODETYPE_IDENTIFIER     ,//= 16,
    &&L_NODETYPE_NIL            ,//= 17,
    &&L_NODETYPE_UNDEFINED      ,//= 18,
    &&L_NODETYPE_CLOSURE        ,//= 19,
    &&L_NODETYPE_PARAMS         //= 20
};
goto *ops[_VM_Program[offset]];

L_NODETYPE_NULL:         return _VM_Evaluate_Null       (offset, env);
L_NODETYPE_PROGRAM:      return _VM_Evaluate_Program    (offset, env);
L_NODETYPE_WRAPPED:      return NULL; // never occurs  
L_NODETYPE_IF_THEN:      return _VM_Evaluate_IfThen     (offset, env);
L_NODETYPE_IF_THEN_ELSE: return _VM_Evaluate_IfThenElse (offset, env);
L_NODETYPE_WHILE_LOOP:   return _VM_Evaluate_WhileLoop  (offset, env);
L_NODETYPE_LOOP_WHILE:   return _VM_Evaluate_LoopWhile  (offset, env);
L_NODETYPE_DO:           return _VM_Evaluate_Do         (offset, env);
L_NODETYPE_ASYNC:        return _VM_Evaluate_Async      (offset, env);
L_NODETYPE_ASSIGN:       return _VM_Evaluate_Assign     (offset, env);
L_NODETYPE_CALL:         return _VM_Evaluate_Call       (offset, env);
L_NODETYPE_ACCESS:       return _VM_Evaluate_Access     (offset, env);
L_NODETYPE_BLOCK:        return _VM_Evaluate_Block      (offset, env);
L_NODETYPE_SEQUENCE:     return _VM_Evaluate_Sequence   (offset, env);
L_NODETYPE_VALUE:        return NULL; // never occurs
L_NODETYPE_NUMBER:       return _VM_Evaluate_Number     (offset, env);
L_NODETYPE_IDENTIFIER:   return _VM_Evaluate_Identifier (offset, env);
L_NODETYPE_NIL:          return _VM_Evaluate_Nil        (offset, env);
L_NODETYPE_UNDEFINED:    return _VM_Evaluate_Undefined  (offset, env);
L_NODETYPE_CLOSURE:      return _VM_Evaluate_Closure    (offset, env);
L_NODETYPE_PARAMS:       return _VM_Evaluate_Params     (offset, env);
*/
        
    
