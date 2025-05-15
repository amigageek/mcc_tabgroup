#include "MUI/TabGroup_mcc.h"
#include "Common.h"

#include <clib/macros.h>
#include <proto/alib.h>
#include <proto/diskfont.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <stdlib.h>
#include <string.h>

// User-configurable padding parameters between text and edge
#define MIN_PAD_WIDTH 12
#define MIN_PAD_TOP 4
#define MIN_PAD_BOTTOM 2

typedef struct {
    Object* tab_group;
    Object* parent_group;
    Object* page;
    struct TextFont* font;
    ULONG user_data;
    STRPTR title;
    ULONG index;
    ULONG active_background_spec;
    UWORD pad_width;
    UWORD pad_top;
    UWORD pad_bottom;
    UWORD title_width;
    BOOL active;
    BOOL compact;
} TabData;

static ULONG tab_dispatcher(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") Msg msg);
static ULONG tab_new(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct opSet* msg);
static ULONG tab_dispose(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG tab_get(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct opGet* msg);
static ULONG tab_set(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct opSet* msg);
static ULONG tab_setup(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") Msg msg);
static ULONG tab_cleanup(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") Msg msg);
static ULONG tab_ask_min_max(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_AskMinMax* msg);
static ULONG tab_draw(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_Draw* msg);
static ULONG tab_drag_query(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_DragQuery* msg);
static ULONG tab_drag_drop(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_DragDrop* msg);
static void set_local_tags(Object* instance, TabData* data, struct TagItem* attr_vals);

struct MUI_CustomClass* tab_make_class(void) {
    return MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(TabData), (APTR)tab_dispatcher);
}

static ULONG tab_dispatcher(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") Msg msg) {
    switch (msg->MethodID) {
    case OM_NEW: return tab_new(cl, obj, (struct opSet*)msg);
    case OM_DISPOSE: return tab_dispose(cl, obj, msg);
    case OM_GET: return tab_get(cl, obj, (struct opGet*)msg);
    case OM_SET: return tab_set(cl, obj, (struct opSet*)msg);
    case OM_UPDATE: return tab_set(cl, obj, (struct opSet*)msg);
    case MUIM_Setup: return tab_setup(cl, obj, msg);
    case MUIM_Cleanup: return tab_cleanup(cl, obj, msg);
    case MUIM_AskMinMax: return tab_ask_min_max(cl, obj, (struct MUIP_AskMinMax*)msg);
    case MUIM_Draw: return tab_draw(cl, obj, (struct MUIP_Draw*)msg);
    case MUIM_DragQuery: return tab_drag_query(cl, obj, (struct MUIP_DragQuery*)msg);
    case MUIM_DragDrop: return tab_drag_drop(cl, obj, (struct MUIP_DragDrop*)msg);
    }

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_new(__reg("a0") struct IClass *cl, __reg("a2") Object *trueclass, __reg("a1") struct opSet* msg) {
    Object* obj = (Object*)DoSuperNew(cl, trueclass,
        MUIA_Dropable, TRUE,
        MUIA_InputMode, MUIV_InputMode_RelVerify,
        MUIA_FillArea, FALSE,
        MUIA_ShowSelState, FALSE,
        MUIA_Font, MUIV_Font_Title,
        TAG_MORE, msg->ops_AttrList);

    if (! obj) {
        return 0;
    }

    TabData* data = INST_DATA(cl, obj);

    *data = (TabData) {
        .tab_group = (Object*)FindTagItem(MUIA_TabGroup_Tab_TabGroup, msg->ops_AttrList)->ti_Data,
    };

    set_local_tags(obj, data, msg->ops_AttrList);

    if (! data->title) {
        CoerceMethod(cl, obj, OM_DISPOSE);
        return 0;
    }

    return (ULONG)obj;
}

static ULONG tab_dispose(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabData* data = INST_DATA(cl, obj);

    FreeVec(data->title);

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_get(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct opGet* msg) {
    TabData* data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID) {
    case MUIA_TabGroup_Tab_TabGroup: *msg->opg_Storage = (ULONG)data->tab_group; return TRUE;
    case MUIA_TabGroup_Tab_Title: *msg->opg_Storage = (ULONG)data->title; return TRUE;
    case MUIA_TabGroup_Tab_Page: *msg->opg_Storage = (ULONG)data->page; return TRUE;
    case MUIA_TabGroup_Tab_UserData: *msg->opg_Storage = (ULONG)data->user_data; return TRUE;
    case MUIA_TabGroup_Tab_Index: *msg->opg_Storage = (ULONG)data->index; return TRUE;
    case MUIA_TabGroup_Tab_Active: *msg->opg_Storage = (ULONG)data->active; return TRUE;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG tab_set(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct opSet* msg) {
    TabData* data = INST_DATA(cl, obj);

    set_local_tags(obj, data, msg->ops_AttrList);

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG tab_setup(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") Msg msg) {
    TabData* data = INST_DATA(cl, obj);

    data->active_background_spec = MUII_RegisterBack;
    DoMethod(obj, MUIM_GetConfigItem, MUICFG_TabGroup_Background, &data->active_background_spec);

    if (data->active) {
        set(obj, MUIA_Background, data->active_background_spec);
    }

    // Superclass setup must occur after changing MUIA_Background
    if (! DoSuperMethodA(cl, obj, msg)) {
        return FALSE;
    }

    get(obj, MUIA_Parent, &data->parent_group);

    STRPTR font_spec;

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_TabGroup_Font, &font_spec)) {
        UBYTE font_name[MAX_FONT_SPEC_SIZE];
        strncpy(font_name, font_spec, MAX_FONT_SPEC_SIZE);

        UWORD font_ysize = 8;
        UBYTE* delimiter;

        if (delimiter = strchr(font_name, '/')) {
            font_ysize = atoi(delimiter + 1);
            *delimiter = 0;
        }

        strncat(font_name, ".font", MAX_FONT_SPEC_SIZE);

        struct TextAttr text_attr = {
            .ta_Name = font_name,
            .ta_YSize = font_ysize,
        };

        data->font = OpenDiskFont(&text_attr);
    }

    // If the user did not specify a font, or if it failed to load, use the default
    if (! data->font) {
        data->font = _font(obj);
    }

    data->pad_width = MIN_PAD_WIDTH;
    data->pad_top = MIN_PAD_TOP;
    data->pad_bottom = MIN_PAD_BOTTOM;
    data->compact = TAB_DEFAULT_COMPACT;

    ULONG* config;

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_TabGroup_Horizontal_Padding, &config)) {
        data->pad_width += *config * 2;
    } else {
        data->pad_width += TAB_DEFAULT_EXTRA_PAD_HORIZ * 2;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_TabGroup_Vertical_Padding, &config)) {
        data->pad_top += *config;
        data->pad_bottom += *config;
    } else {
        data->pad_top += TAB_DEFAULT_EXTRA_PAD_VERT;
        data->pad_bottom += TAB_DEFAULT_EXTRA_PAD_VERT;
    }

    if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_TabGroup_CompactTabs, &config)) {
        data->compact = *config;
    }

    return TRUE;
}

static ULONG tab_cleanup(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    TabData *data = INST_DATA(cl, obj);

    if (data->font != _font(obj)) {
        CloseFont(data->font);
        data->font = NULL;
    }

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG tab_ask_min_max(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_AskMinMax* msg) {
    TabData* data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);

    struct MUI_MinMax* mmi = (struct MUI_MinMax*)msg->MinMaxInfo;

    struct RastPort rp;
    InitRastPort(&rp);
    SetFont(&rp, data->font);

    data->title_width = TextLength(&rp, data->title, strlen(data->title));

    UWORD default_width = data->title_width + data->pad_width;
    UWORD tab_height = data->pad_top + data->font->tf_YSize + data->pad_bottom;

    mmi->MinWidth += data->pad_width;
    mmi->DefWidth += default_width;
    mmi->MaxWidth = (data->compact ? default_width : MUI_MAXMAX);

    mmi->MinHeight += tab_height;
    mmi->DefHeight += tab_height;
    mmi->MaxHeight += tab_height;
    
    return TRUE;
}

static ULONG tab_draw(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_Draw* msg) {
    TabData* data = INST_DATA(cl, obj);

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (! (msg->flags & MADF_DRAWOBJECT)) {
        return TRUE;
    }

    // MUI 3.9+ asks us to draw to a different RastPort when being dragged.
    // Prefer not to draw the background when dragged but 3.8 has no way to detect.
    BOOL draw_parent_bg = _rp(obj) == _rp(data->parent_group);

    // When tab is active: inset by 1 pixel left/right, 2 pixels down
    BOOL active = data->active;
    UWORD inset = (active ? 0 : 1);
    UWORD left = _mleft(obj) + inset;
    UWORD right = _mright(obj) - inset;
    UWORD top = _mtop(obj) + (inset * 2);
    UWORD bottom = _mbottom(obj);

    // Fill tab with background color (overdraws by 2 pixels)
    draw_background(obj, left + 1, top + 1, right - left - 1, bottom - top);

    // Restore parent background along tab diagonal left/right edges
    for (UWORD row = 0; row < 3; ++ row) {
        UWORD y = top + row;
        UWORD left_width = 4 - row + inset; // {4,3,2} + inset

        UWORD right_width = left_width + (row == 0 ? 1 : 0); // {5,3,2} + inset
        UWORD right_x = right - (3 - row + (row == 0 ? 1 : 0)); // right - {4,2,1}

        if (draw_parent_bg) {
            draw_background(data->parent_group, _mleft(obj), y, left_width, 1);
            draw_background(data->parent_group, right_x, y, right_width, 1);
        }
    }

    // Restore parent background to left/right of tab
    if (draw_parent_bg) {
        draw_background(data->parent_group, _mleft(obj), top + 3, 1 + inset, bottom - top - 3 + inset);
        draw_background(data->parent_group, right - 0,   top + 3, 1 + inset, bottom - top - 3 + inset);

        if (! active) {
            // Restore parent background above tab
            draw_background(data->parent_group, _mleft(obj), _mtop(obj), _mwidth(obj), 2);
        }
    }

    struct RastPort* rp = _rp(obj);
    UWORD* pens = _pens(obj);

    // Center text within the available space, clipping if needed
    UWORD avail_width = _mwidth(obj) - data->pad_width;

    if (avail_width > 0) {
        BOOL clip_text = avail_width < data->title_width;
        UWORD clipped_width = clip_text ? avail_width : data->title_width;
        UWORD title_left = _mleft(obj) + ((_mwidth(obj) - clipped_width) / 2);
        UWORD title_top = _mtop(obj) + data->pad_top + (active ? 0 : 1);
        APTR clipping;

        if (clip_text) {
            clipping = MUI_AddClipping(muiRenderInfo(obj), title_left, title_top, clipped_width, data->font->tf_YSize);
        }

        // Draw the centered text
        SetAPen(rp, pens[MPEN_TEXT]);
        SetFont(rp, data->font);
        Move(rp, title_left, title_top + data->font->tf_Baseline);
        Text(rp, data->title, strlen(data->title));

        if (clip_text) {
            MUI_RemoveClipping(muiRenderInfo(obj), clipping);
        }
    }

    // The rendering below mimics the MUI Register object

    // Draw left/top frame edges and curve between them
    SetAPen(rp, pens[MPEN_SHINE]);
    WritePixel(rp, left + 2, top + 3);
    WritePixel(rp, left + 2, top + 2);
    WritePixel(rp, left + 3, top + 1);
    WritePixel(rp, left + 4, top + 1);
    Move(rp, left + 1, bottom - (active ? 1 : 0));
    Draw(rp, left + 1, top + 4);
    Move(rp, left + 5, top);
    Draw(rp, right - 5, top);

    if (active) {
        WritePixel(rp, left, bottom);
    }

    // Draw background detail pixels inside tab
    SetAPen(rp, pens[MPEN_HALFSHINE]);
    WritePixel(rp, left + 3, top + 2);
    WritePixel(rp, left + 1, top + 3);
    WritePixel(rp, left + 4, top);
    Move(rp, left + 2, top + 4);
    Draw(rp, left + 5, top + 1);

    // Draw right edge and detail pixels
    SetAPen(rp, pens[MPEN_SHADOW]);
    WritePixel(rp, right - 2, top + 2);
    Move(rp, right - 1, top + 4);
    Draw(rp, right - 1, bottom - (active ? 1 : 0));

    if (active) {
        WritePixel(rp, right, bottom);
    }

    // Draw detail pixels
    SetAPen(rp, pens[MPEN_FILL]);
    WritePixel(rp, right - 3, top + 1);
    WritePixel(rp, right - 2, top + 3);
    WritePixel(rp, right - 1, top + 3);

    return TRUE;
}

static ULONG tab_drag_query(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_DragQuery* msg) {
    TabData* data = INST_DATA(cl, obj);

    // Cannot drag the tab onto itself
    if (msg->obj == obj) {
        return MUIV_DragQuery_Refuse;
    }

    // Cannot drag the tab into a different TabGroup
    Object* tab_group;

    if (get(msg->obj, MUIA_TabGroup_Tab_TabGroup, &tab_group) && (tab_group == data->tab_group)) {
        return MUIV_DragQuery_Accept;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

static ULONG tab_drag_drop(__reg("a0") struct IClass *cl, __reg("a2") Object *obj, __reg("a1") struct MUIP_DragDrop* msg) {
    TabData* data = INST_DATA(cl, obj);

    DoMethod(data->tab_group, MUIM_TabGroup_SwapTabs, msg->obj, obj);

    return TRUE;
}

static void set_local_tags(Object* instance, TabData* data, struct TagItem* taglist) {
    struct TagItem* iter_tag = taglist;

    for (struct TagItem* tagitem; tagitem = NextTagItem(&iter_tag); ) {
        switch (tagitem->ti_Tag) {
        case MUIA_TabGroup_Tab_Page: data->page = (Object*)tagitem->ti_Data; break;
        case MUIA_TabGroup_Tab_UserData: data->user_data = tagitem->ti_Data; break;
        case MUIA_TabGroup_Tab_Index: data->index = tagitem->ti_Data; break;

        case MUIA_TabGroup_Tab_Active:
            data->active = tagitem->ti_Data;
            set(instance, MUIA_Background, data->active ? data->active_background_spec : MUII_BACKGROUND);

            // Avoid redundant changes to MUIA_TabGroup_ActiveTab by ignoring mouse presses on the active tab
            if (data->active) {
                DoMethod(instance, MUIM_KillNotify, MUIA_Pressed);
            } else {
                DoMethod(instance, MUIM_Notify, MUIA_Pressed, TRUE,
                    data->tab_group, 3, MUIM_Set, MUIA_TabGroup_ActiveTab, instance);
            }

            break;

        case MUIA_TabGroup_Tab_Title: {
            STRPTR title = (STRPTR)tagitem->ti_Data;
            STRPTR title_copy = (STRPTR)AllocVec(strlen(title) + 1, 0);

            if (title_copy) {
                BOOL relayout_needed = data->title != NULL;

                strcpy(title_copy, title);
                FreeVec(data->title);
                data->title = title_copy;

                if (relayout_needed) {
                    // If the title changes after initial MUIM_Setup then force re-layout
                    DoMethod(data->parent_group, MUIM_Group_InitChange);
                    DoMethod(data->parent_group, MUIM_Group_ExitChange);
                }
            }

            break;
        }
        }
    }
}
