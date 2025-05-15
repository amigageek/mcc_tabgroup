#include "Common.h"

#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

void insert_member(Object* parent, struct List* member_list, Object* new_member, UWORD insert_index) {
    struct List move_list;
    NewList(&move_list);

    APTR iter_node = member_list->lh_Head;
    Object* iter_member;

    for (UWORD iter_index = 0; iter_member = NextObject(&iter_node); ++ iter_index) {
        if (iter_index >= insert_index) {
            DoMethod(parent, OM_REMMEMBER, iter_member);
            DoMethod(iter_member, OM_ADDTAIL, &move_list);
        }
    }

    DoMethod(parent, OM_ADDMEMBER, new_member);

    for (iter_node = move_list.lh_Head; iter_member = NextObject(&iter_node); ) {
        DoMethod(parent, OM_ADDMEMBER, iter_member);
    }
}

Object* nth_object(struct List* list, UWORD object_index) {
    APTR iter_node = list->lh_Head;
    Object* object;

    for (UWORD iter_index = 0; iter_index <= object_index; ++ iter_index) {
        object = NextObject(&iter_node);
    }

    return object;
}

ULONG DoSuperNew(struct IClass* cl, Object* obj, ULONG tag1, ...) {
    return DoSuperMethod(cl, obj, OM_NEW, &tag1, NULL);
}

void draw_background(Object* instance, LONG left, LONG top, LONG width, LONG height) {
    LONG xoffset = 0;
    LONG yoffset = 0;

    if (MUIMasterBase->lib_Version == 19) {
        // MUI 3.8 misinterprets the offset fields, fixed in 3.9+.
        xoffset = left;
        yoffset = top;
    }

    DoMethod(instance, MUIM_DrawBackground, left, top, width, height, xoffset, yoffset, 0);
}
