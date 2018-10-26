
#ifndef H_LIST
#define H_LIST

/* ListElement */

    ListElement* NewListElement         ();
    ListElement* OBJ_LE_Copy            (ListElement* me, ListElement* prev);
    
/* List */    
    
    bool        OBJ_LIST_IsEmpty        (Object* list);    
    bool        OBJ_LIST_IsSingleton    (Object* list);
    bool        OBJ_LIST_IsPipe         (Object* list);
    void        OBJ_LIST_AppendElement  (Object* list, Object* o);

#endif

