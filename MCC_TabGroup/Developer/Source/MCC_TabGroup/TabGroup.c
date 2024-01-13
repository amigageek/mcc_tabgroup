#include "MUI/TabGroup_mcc.h"
#include "Common.h"

#include <clib/macros.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

// Default spacing matches the Register object
#define TAB_GROUP_FRAME_SIZE 1
#define PAGE_GROUP_INNER_HORIZ 6
#define PAGE_GROUP_INNER_VERT 4

#define CLASS MUIC_TabGroup
#define SUPERCLASS MUIC_Group

typedef struct Data {
    struct MUI_EventHandlerNode event_handler_node;
    Object* instance;
    Object* tabs_group;
    Object* page_group;
    Object* empty_page;
    Object* active_tab;
    struct List* tab_list;
    struct List* page_list;
    struct Hook* tabs_layout_hook;
    ULONG num_tabs;
    ULONG active_tab_index;
    BOOL page_mode;
    BOOL draggable_tabs;
} TabGroupData;

#include "MCCLib.c"

static BOOL ClassInitFunc(const struct Library* const base);
static void ClassExitFunc(const struct Library* const base);
static ULONG Dispatcher(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG tab_group_new(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct opSet* msg);
static ULONG tab_group_dispose(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG tab_group_get(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct opGet* msg);
static ULONG tab_group_set(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct opSet* msg);
static ULONG tab_group_setup(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG tab_group_cleanup(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG tab_group_notify(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_Notify* msg);
static ULONG tab_group_handle_event(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_HandleEvent* msg);
static ULONG tab_group_draw(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static Object* tab_group_add(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_TabGroup_AddTab* msg);
static ULONG tab_group_remove(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_TabGroup_RemoveTab* msg);
static ULONG tab_group_swap(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_TabGroup_SwapTabs* msg);
static LONG tabs_layout(__reg("a0") struct Hook* hook, __reg("a2") Object* obj, __reg("a1") struct MUI_LayoutMsg* msg);
static void set_active_tab(TabGroupData* data, Object* tab_or_special);
static Object* resolve_special_tab_value(TabGroupData* data, Object* tab_or_special);
static ULONG calculate_tab_index(TabGroupData* data, LONG index_or_special, BOOL allow_end);
static void reindex_tabs(TabGroupData* data);

// Attributes delegated to the page group
static Tag page_group_tags[] = {
    MUIA_Group_ActivePage,
    MUIA_Group_Child,
    MUIA_Group_ChildList,
    MUIA_Group_Columns,
    MUIA_Group_Horiz,
    MUIA_Group_HorizSpacing,
    MUIA_Group_LayoutHook,
    MUIA_Group_PageMode,
    MUIA_Group_Rows,
    MUIA_Group_SameHeight,
    MUIA_Group_SameSize,
    MUIA_Group_SameWidth,
    MUIA_Group_Spacing,
    MUIA_Group_VertSpacing,
    MUIA_InnerBottom,
    MUIA_InnerLeft,
    MUIA_InnerRight,
    MUIA_InnerTop,
    TAG_DONE
};

static struct MUI_CustomClass* tab_class;
struct Library* DiskfontBase;

static BOOL ClassInitFunc(const struct Library* const base) {
    if (! (DiskfontBase = OpenLibrary("diskfont.library", 0))) {
        return FALSE;
    }

    tab_class = tab_make_class();
    return tab_class != NULL;
}

static void ClassExitFunc(const struct Library* const base) {
    if (tab_class) {
        MUI_DeleteCustomClass(tab_class);
    }

    CloseLibrary(DiskfontBase);
}

static ULONG Dispatcher(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    switch (msg->MethodID) {
    case OM_NEW: return tab_group_new(cl, obj, (struct opSet*)msg);
    case OM_DISPOSE: return tab_group_dispose(cl, obj, msg);
    case OM_GET: return tab_group_get(cl, obj, (struct opGet*)msg);
    case OM_SET: return tab_group_set(cl, obj, (struct opSet*)msg);
    case OM_UPDATE: return tab_group_set(cl, obj, (struct opSet*)msg);
    case MUIM_Setup: return tab_group_setup(cl, obj, msg);
    case MUIM_Cleanup: return tab_group_cleanup(cl, obj, msg);
    case MUIM_Notify: return tab_group_notify(cl, obj, (struct MUIP_Notify*)msg);
    case MUIM_HandleEvent: return tab_group_handle_event(cl, obj, (struct MUIP_HandleEvent*)msg);
    case MUIM_Draw: return tab_group_draw(cl, obj, msg);
    case MUIM_TabGroup_AddTab: return (ULONG)tab_group_add(cl, obj, (struct MUIP_TabGroup_AddTab*)msg);
    case MUIM_TabGroup_RemoveTab: return tab_group_remove(cl, obj, (struct MUIP_TabGroup_RemoveTab*)msg);
    case MUIM_TabGroup_SwapTabs: return tab_group_swap(cl, obj, (struct MUIP_TabGroup_SwapTabs*)msg);

    // Methods delegated to page group
    case OM_ADDMEMBER:
    case OM_REMMEMBER:
    case MUIM_Group_InitChange:
    case MUIM_Group_ExitChange:
        return DoMethodA(data->page_group, msg);
    }

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_group_new(__reg("a0") struct IClass* cl, __reg("a2") Object* trueclass, __reg("a1") struct opSet* msg) {
    struct Hook* tabs_layout_hook = NULL;
    struct TagItem* super_taglist = NULL;
    struct TagItem* page_group_taglist = NULL;
    Object* obj = NULL;

    if (! (tabs_layout_hook = AllocVec(sizeof(struct Hook), MEMF_CLEAR))) {
        goto exit;
    }

    if (! (super_taglist = CloneTagItems(msg->ops_AttrList))) {
        goto exit;
    }

    if (! (page_group_taglist = CloneTagItems(msg->ops_AttrList))) {
        goto exit;
    }

    FilterTagItems(super_taglist, page_group_tags, TAGFILTER_NOT);
    FilterTagItems(page_group_taglist, page_group_tags, TAGFILTER_AND);

    BOOL page_mode = GetTagData(MUIA_TabGroup_PageMode, TRUE, msg->ops_AttrList);
    BOOL draggable_tabs = GetTagData(MUIA_TabGroup_DraggableTabs, FALSE, msg->ops_AttrList);

    Object* tabs_group;
    Object* page_group;
    Object* empty_page;

    obj = (Object*)DoSuperNew(cl, trueclass,
        MUIA_InnerLeft, TAB_GROUP_FRAME_SIZE,
        MUIA_InnerRight, TAB_GROUP_FRAME_SIZE,
        MUIA_InnerBottom, TAB_GROUP_FRAME_SIZE,
        MUIA_Group_Spacing, TAB_GROUP_FRAME_SIZE,
        Child, tabs_group = HGroup,
            MUIA_Group_Spacing, 0,
            MUIA_Group_LayoutHook, tabs_layout_hook,
        End,
        Child, page_group = VGroup,
            MUIA_InnerLeft, PAGE_GROUP_INNER_HORIZ,
            MUIA_InnerRight, PAGE_GROUP_INNER_HORIZ,
            MUIA_InnerTop, PAGE_GROUP_INNER_VERT,
            MUIA_InnerBottom, PAGE_GROUP_INNER_VERT,
            MUIA_Group_PageMode, page_mode,
            TAG_SKIP, page_mode ? 0 : 1,
            Child, empty_page = (page_mode ? HSpace(0) : NULL),
            TAG_MORE, page_group_taglist,
        End,
        TAG_MORE, super_taglist);

    if (! obj) {
        goto exit;
    }

    TabGroupData* data = INST_DATA(cl, obj);

    *data = (TabGroupData){
        .event_handler_node = {
            .ehn_Events = IDCMP_RAWKEY,
            .ehn_Object = obj,
            .ehn_Class = cl,
        },
        .instance = obj,
        .tabs_group = tabs_group,
        .page_group = page_group,
        .empty_page = empty_page,
        .active_tab = NULL,
        .tabs_layout_hook = tabs_layout_hook,
        .num_tabs = 0,
        .active_tab_index = 0,
        .page_mode = page_mode,
        .draggable_tabs = draggable_tabs,
    };

    get(tabs_group, MUIA_Group_ChildList, &data->tab_list);
    get(page_group, MUIA_Group_ChildList, &data->page_list);

    *tabs_layout_hook = (struct Hook){
        .h_Entry = (ULONG (*)())tabs_layout,
        .h_Data = data,
    };

exit:
    if (! obj) {
        FreeVec(tabs_layout_hook);
    }

    FreeTagItems(page_group_taglist);
    FreeTagItems(super_taglist);

    return (ULONG)obj;
}

static ULONG tab_group_dispose(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    if (data->page_mode && (data->num_tabs > 0)) {
        MUI_DisposeObject(data->empty_page);
    }

    FreeVec(data->tabs_layout_hook);

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_group_get(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct opGet* msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID) {
    case MUIA_TabGroup_ActiveTab: *msg->opg_Storage = (ULONG)data->active_tab; return TRUE;
    case MUIA_TabGroup_NumTabs: *msg->opg_Storage = data->num_tabs; return TRUE;
    case MUIA_TabGroup_TabList: *msg->opg_Storage = (ULONG)data->tab_list; return TRUE;
    }

    if (TagInArray(msg->opg_AttrID, page_group_tags)) {
        return DoMethodA(data->page_group, (Msg)msg);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG tab_group_set(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct opSet* msg) {
    TabGroupData* data = INST_DATA(cl, obj);
    struct TagItem* copy_taglist;

    if (! (copy_taglist = CloneTagItems(msg->ops_AttrList))) {
        return FALSE;
    }

    if (FilterTagItems(copy_taglist, page_group_tags, TAGFILTER_AND)) {
        SetAttrsA(data->page_group, copy_taglist);
    }

    FreeTagItems(copy_taglist);

    struct TagItem* active_tab_tagitem;

    if (active_tab_tagitem = FindTagItem(MUIA_TabGroup_ActiveTab, msg->ops_AttrList)) {
        set_active_tab(data, (Object*)active_tab_tagitem->ti_Data);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG tab_group_setup(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    ULONG background_spec = MUII_RegisterBack;
    DoMethod(obj, MUIM_GetConfigItem, MUICFG_TabGroup_Background, &background_spec);

    set(data->page_group, MUIA_Background, background_spec);

    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->event_handler_node);

    // Superclass setup must occur after changing group member MUIA_Background
    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_group_cleanup(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->event_handler_node);

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_group_notify(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_Notify* msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    if (TagInArray(msg->TrigAttr, page_group_tags)) {
        return DoMethodA(data->page_group, (Msg)msg);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG tab_group_handle_event(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_HandleEvent* msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    if (msg->muikey == MUIKEY_NONE) {
        return FALSE;
    }

    LONG tab_special;

    switch (msg->muikey) {
    case MUIKEY_LEFT:
        tab_special = MUIV_TabGroup_Tab_Prev;
        break;

    case MUIKEY_RIGHT:
        tab_special = MUIV_TabGroup_Tab_Next;
        break;

    case MUIKEY_WORDLEFT:
    case MUIKEY_LINESTART:
        tab_special = MUIV_TabGroup_Tab_First;
        break;

    case MUIKEY_WORDRIGHT:
    case MUIKEY_LINEEND:
        tab_special = MUIV_TabGroup_Tab_Last;
        break;

    case MUIKEY_PRESS:
    case MUIKEY_TOGGLE:
        tab_special = MUIV_TabGroup_Tab_Advance;
        break;

    default:
        return DoSuperMethodA(cl, obj, (Msg)msg);
    }

    // In case the user notifies on MUIA_TabGroup_ActiveTab with MUIV_TriggerValue
    // resolve the special value to the Tab object. This doesn't prevent the user
    // setting a special value directly, but the behavior is clearer in that case.
    Object* active_tab = resolve_special_tab_value(data, (Object*)tab_special);

    set(obj, MUIA_TabGroup_ActiveTab, active_tab);

    return MUI_EventHandlerRC_Eat;
}

static ULONG tab_group_draw(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, msg);

    // There is an unavoidable redraw after adding the first tab but before activating it
    if ((data->num_tabs == 0) || (! data->active_tab)) {
        return TRUE;
    }

    struct RastPort* rp = _rp(obj);
    UWORD* pens = _pens(obj);

    ULONG active_tab_left, active_tab_right, tabs_bottom;
    get(data->active_tab, MUIA_LeftEdge, &active_tab_left);
    get(data->active_tab, MUIA_RightEdge, &active_tab_right);
    get(data->active_tab, MUIA_BottomEdge, &tabs_bottom);

    UWORD frame_left = _mleft(obj) - TAB_GROUP_FRAME_SIZE;
    UWORD frame_right = _mright(obj) + TAB_GROUP_FRAME_SIZE;
    UWORD frame_top = tabs_bottom + TAB_GROUP_FRAME_SIZE;
    UWORD frame_bottom = _mbottom(obj) + TAB_GROUP_FRAME_SIZE;

    // Erase top frame edge below active tab
    UWORD active_tab_width = active_tab_right - active_tab_left + 1;

    DoMethod(data->page_group, MUIM_DrawBackground, active_tab_left, frame_top,
        active_tab_width, TAB_GROUP_FRAME_SIZE, active_tab_left, frame_top, 0);

    // Draw left frame edge
    SetAPen(rp, pens[MPEN_SHINE]);
    Move(rp, frame_left, frame_bottom - TAB_GROUP_FRAME_SIZE);
    Draw(rp, frame_left, frame_top);

    if (data->active_tab_index > 0) {
        // Draw top frame edge to left of active tab
        Draw(rp, active_tab_left - 1, frame_top);
    }

    BOOL gap_after_last_tab = active_tab_right < _mright(obj);

    if (gap_after_last_tab) {
        // Draw top frame edge to right of active tab
        Move(rp, active_tab_right + 1, frame_top);
        Draw(rp, frame_right - TAB_GROUP_FRAME_SIZE, frame_top);
    }

    // Draw bottom and right frame edges
    SetAPen(rp, pens[MPEN_SHADOW]);
    Move(rp, frame_left + TAB_GROUP_FRAME_SIZE, frame_bottom);
    Draw(rp, frame_right, frame_bottom);
    Draw(rp, frame_right, frame_top + (gap_after_last_tab ? TAB_GROUP_FRAME_SIZE : 0));

    // Draw bottom-left and top-right frame corners
    SetAPen(rp, pens[MPEN_BACKGROUND]);
    WritePixel(rp, frame_left, frame_bottom);

    if (gap_after_last_tab) {
        WritePixel(rp, frame_right, frame_top);
    }

    return TRUE;
}

static Object* tab_group_add(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_TabGroup_AddTab* msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    if (data->page_mode && (! msg->page)) {
        return NULL;
    }

    ULONG insert_index = calculate_tab_index(data, msg->index, TRUE);

    Object* new_tab = NewObject(tab_class->mcc_Class, NULL,
        MUIA_Draggable, data->draggable_tabs,
        MUIA_TabGroup_Tab_TabGroup, obj,
        MUIA_TabGroup_Tab_Title, msg->title,
        MUIA_TabGroup_Tab_Page, msg->page,
        MUIA_TabGroup_Tab_UserData, msg->user_data,
        MUIA_TabGroup_Tab_Index, insert_index,
        MUIA_TabGroup_Tab_Active, msg->active,
        TAG_DONE);

    if (! new_tab) {
        return NULL;
    }

    BOOL first_tab = (data->num_tabs == 0);
    ++ data->num_tabs;

    DoMethod(data->tabs_group, MUIM_Group_InitChange);
    insert_member(data->tabs_group, data->tab_list, new_tab, insert_index);
    DoMethod(data->tabs_group, MUIM_Group_ExitChange);

    if (data->page_mode) {
        DoMethod(data->page_group, MUIM_Group_InitChange);

        if (first_tab) {
            // A page group must have at least one member, remove the placeholder
            DoMethod(data->page_group, OM_REMMEMBER, data->empty_page);
        }

        insert_member(data->page_group, data->page_list, msg->page, insert_index);
    }

    // If we inserted behind an existing tab then recalculate all tab indices
    if (insert_index < (data->num_tabs - 1)) {
        reindex_tabs(data);
    }

    if (msg->active || first_tab) {
        // Will also redraw the top frame
        set(data->instance, MUIA_TabGroup_ActiveTab, new_tab);
    } else {
        // Redraw the top frame because tab sizes have changed
        MUI_Redraw(data->instance, MADF_DRAWUPDATE);
    }

    if (data->page_mode) {
        // ExitChange must be deferred until MUIA_Group_ActivePage matches the page list
        DoMethod(data->page_group, MUIM_Group_ExitChange);
    }

    return new_tab;
}

static ULONG tab_group_remove(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_TabGroup_RemoveTab* msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    Object* old_tab = resolve_special_tab_value(data, msg->tab);

    ULONG tab_index;
    get(old_tab, MUIA_TabGroup_Tab_Index, &tab_index);

    -- data->num_tabs;
    BOOL last_tab = (data->num_tabs == 0);

    DoMethod(data->tabs_group, MUIM_Group_InitChange);
    DoMethod(data->tabs_group, OM_REMMEMBER, old_tab);
    DoMethod(data->tabs_group, MUIM_Group_ExitChange);

    if (data->page_mode) {
        Object* old_page;
        get(old_tab, MUIA_TabGroup_Tab_Page, &old_page);

        DoMethod(data->page_group, MUIM_Group_InitChange);
        DoMethod(data->page_group, OM_REMMEMBER, old_page);

        if (last_tab) {
            // A page group must have at least one member, add the placeholder
            DoMethod(data->page_group, OM_ADDMEMBER, data->empty_page);
        }
    }

    // If we removed behind an existing tab then recalculate all tab indices
    if (tab_index < data->num_tabs) {
        reindex_tabs(data);
    }

    if (last_tab) {
        data->active_tab = NULL;
    } else if (old_tab == data->active_tab) {
        // Try to activate the tab in the same position as the removed one
        Object* new_active_tab = nth_object(data->tab_list, MIN(tab_index, data->num_tabs - 1));

        // Will also redraw the top frame
        set(data->instance, MUIA_TabGroup_ActiveTab, new_active_tab);
    } else {
        // Redraw the top frame because tab sizes have changed
        MUI_Redraw(data->instance, MADF_DRAWUPDATE);
    }

    if (data->page_mode) {
        // ExitChange must be deferred until MUIA_Group_ActivePage matches the page list
        DoMethod(data->page_group, MUIM_Group_ExitChange);
    }

    MUI_DisposeObject(old_tab);

    return TRUE;
}

static ULONG tab_group_swap(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_TabGroup_SwapTabs* msg) {
    TabGroupData* data = INST_DATA(cl, obj);

    Object* tabs[] = { msg->tab1, msg->tab2 };
    Object* pages[2];
    ULONG indices[2];

    for (UWORD i = 0; i < 2; ++ i) {
        get(tabs[i], MUIA_TabGroup_Tab_Page, &pages[i]);
        get(tabs[i], MUIA_TabGroup_Tab_Index, &indices[i]);
    }

    // Order tabs so that reinserting both does not affect either's insertion index
    if (indices[0] > indices[1]) {
        SWAP(Object*, tabs[0], tabs[1]);
        SWAP(Object*, pages[0], pages[1]);
        SWAP(ULONG, indices[0], indices[1]);
    }

    DoMethod(data->tabs_group, MUIM_Group_InitChange);
    DoMethod(data->tabs_group, OM_REMMEMBER, tabs[0]);
    DoMethod(data->tabs_group, OM_REMMEMBER, tabs[1]);
    insert_member(data->tabs_group, data->tab_list, tabs[1], indices[0]);
    insert_member(data->tabs_group, data->tab_list, tabs[0], indices[1]);
    DoMethod(data->tabs_group, MUIM_Group_ExitChange);

    if (data->page_mode) {
        DoMethod(data->page_group, MUIM_Group_InitChange);
        DoMethod(data->page_group, OM_REMMEMBER, pages[0]);
        DoMethod(data->page_group, OM_REMMEMBER, pages[1]);
        insert_member(data->page_group, data->page_list, pages[1], indices[0]);
        insert_member(data->page_group, data->page_list, pages[0], indices[1]);
        DoMethod(data->page_group, MUIM_Group_ExitChange);
    }

    reindex_tabs(data);

    // Redraw the top frame because the active tab has moved
    MUI_Redraw(data->instance, MADF_DRAWUPDATE);

    return TRUE;
}

static LONG tabs_layout(__reg("a0") struct Hook* hook, __reg("a2") Object* obj, __reg("a1") struct MUI_LayoutMsg* msg) {
    switch (msg->lm_Type) {
    case MUILM_MINMAX: {
        UWORD min_width = 0;
        UWORD def_width = 0;
        UWORD fix_height = 0;

        APTR iter_node = msg->lm_Children->mlh_Head;

        // Tabs will be packed together, so sum their sizes for minimum/default
        // Space will fill any remaining width

        for (Object* iter_child; iter_child = NextObject(&iter_node); ) {
            min_width += _minwidth(iter_child);
            def_width += _defwidth(iter_child);
            fix_height = _minheight(iter_child);
        }

        msg->lm_MinMax.MinWidth  = min_width;
        msg->lm_MinMax.MinHeight = fix_height;
        msg->lm_MinMax.DefWidth  = def_width;
        msg->lm_MinMax.DefHeight = fix_height;
        msg->lm_MinMax.MaxWidth  = MUI_MAXMAX;
        msg->lm_MinMax.MaxHeight = fix_height;

        return 0;
        }

    case MUILM_LAYOUT: {
        TabGroupData* data = (TabGroupData*)hook->h_Data;

        if (data->num_tabs == 0) {
            return TRUE;
        }

        // Try to give every tab the same width
        UWORD even_tab_width = msg->lm_Layout.Width / data->num_tabs;
        UWORD tab_left = 0;

        APTR iter_node = msg->lm_Children->mlh_Head;
        Object* iter_child;

        for (UWORD iter_index = 0; iter_child = NextObject(&iter_node); ++ iter_index) {
            // The last tab should fill the remaining space if it would not exceed its maximum width
            // This ensures the right edge lines up with the frame when shrunk below maximum width
            BOOL last_tab = (iter_index == (data->num_tabs - 1));
            UWORD max_tab_width = last_tab ? (msg->lm_Layout.Width - tab_left) : even_tab_width;
            UWORD tab_width = MIN(max_tab_width, _maxwidth(iter_child));

            if (! MUI_Layout(iter_child, tab_left, 0, tab_width, msg->lm_Layout.Height, 0)) {
                return FALSE;
            }

            tab_left += tab_width;
        }

        return TRUE;
    }
    }

    return MUILM_UNKNOWN;
}

static void set_active_tab(TabGroupData* data, Object* tab_or_special) {
    // Not expected but prevent the user from doing this
    if (data->num_tabs == 0) {
        return;
    }

    Object* new_active_tab = resolve_special_tab_value(data, tab_or_special);

    if (data->active_tab != new_active_tab) {
        if (data->active_tab) {
            set(data->active_tab, MUIA_TabGroup_Tab_Active, FALSE);
        }

        data->active_tab = new_active_tab;
        get(new_active_tab, MUIA_TabGroup_Tab_Index, &data->active_tab_index);
        set(new_active_tab, MUIA_TabGroup_Tab_Active, TRUE);

        MUI_Redraw(data->instance, MADF_DRAWUPDATE);

        if (data->page_mode) {
            set(data->page_group, MUIA_Group_ActivePage, data->active_tab_index);
        }
    }
}

static Object* resolve_special_tab_value(TabGroupData* data, Object* tab_or_special) {
    Object* tab = tab_or_special;
    LONG special_value = (LONG)tab_or_special;

    if (special_value <= MUIV_TabGroup_Tab_First &&
        special_value >= MUIV_TabGroup_Tab_Active)
    {
        ULONG tab_index = calculate_tab_index(data, special_value, FALSE);
        tab = nth_object(data->tab_list, tab_index);
    }

    return tab;
}

static ULONG calculate_tab_index(TabGroupData* data, LONG index_or_special, BOOL allow_end) {
    ULONG last_index = data->num_tabs - (allow_end ? 0 : 1);

    switch (index_or_special) {
    case MUIV_TabGroup_Tab_First: return 0;
    case MUIV_TabGroup_Tab_Last: return last_index;
    case MUIV_TabGroup_Tab_Prev: return MAX((LONG)data->active_tab_index - 1, 0);
    case MUIV_TabGroup_Tab_Next: return MIN(data->active_tab_index + 1, last_index);
    case MUIV_TabGroup_Tab_Advance: return ((data->active_tab_index + 1) % (last_index + 1));
    case MUIV_TabGroup_Tab_Active: return data->active_tab_index;
    }

    return MIN(index_or_special, last_index);
}

static void reindex_tabs(TabGroupData* data) {
    APTR iter_node = data->tab_list->lh_Head;

    for (ULONG tab_index = 0; tab_index < data->num_tabs; ++ tab_index) {
        Object* tab = NextObject(&iter_node);
        set(tab, MUIA_TabGroup_Tab_Index, tab_index);

        if (tab == data->active_tab) {
            data->active_tab_index = tab_index;
            set(data->page_group, MUIA_Group_ActivePage, tab_index);
        }
    }
}
