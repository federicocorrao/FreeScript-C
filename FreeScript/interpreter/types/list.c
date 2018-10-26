
    #include "list.h"

    ListElement* NewListElement()
    {
        ListElement* le = (ListElement*) malloc(sizeof(ListElement));
        le -> Value = NULL;
        le -> Next = NULL;
        le -> Prev = NULL;
        return le;
    }
    
    /*ListElement* OBJ_LE_Copy(ListElement* me, ListElement* prev)
    {
        ListElement* le = NewListElement();
        le -> Value = OBJ_Copy(me -> Value);                                    
        le -> Prev  = prev;
        le -> Next  = OBJ_LE_Copy(me -> Next, me);
        return le;
    }*/
    
/* List functions */

    inline bool OBJ_LIST_IsEmpty(Object* list)
    {
        return list -> O.AsList.Length == 0;
    }
        
    inline bool OBJ_LIST_IsSingleton(Object* list)
    {
        return list -> O.AsList.Length == 1;
    }
    
    inline bool OBJ_LIST_IsPipe(Object* list)
    {
        return OBJ_LIST_IsSingleton(list) &&
               OBJ_IsList(list -> O.AsList.First -> Value);    
    }
    
    /**/

    void OBJ_LIST_AppendElement(Object* list, Object* o)
    {
        o = OBJ_Itself(o);
        if(OBJ_IsUndefined(o)) return;
        
        if(OBJ_LIST_IsEmpty(list))
        {
            list -> O.AsList.First = NewListElement();
            list -> O.AsList.First -> Value = o;
            list -> O.AsList.First -> Prev = list -> O.AsList.First;
            list -> O.AsList.Last = list -> O.AsList.First;
        }
        else
        {
            list -> O.AsList.Last -> Next = NewListElement();
            list -> O.AsList.Last -> Next -> Value = o;
            list -> O.AsList.Last -> Next -> Prev = list -> O.AsList.Last;
            list -> O.AsList.Last = list -> O.AsList.Last -> Next;
        }
        (list -> O.AsList.Length) += 1;
    }

    Object* OBJ_LIST_At(Object* list, int index)
    {
        if(index >= list -> O.AsList.Length || index < 0)
            return OBJ_Undefined;
        ListElement* pointer = list -> O.AsList.First;
        int i; for(i = 0; i < index; i++)
            pointer = pointer -> Next;
        return pointer -> Value;
    }


