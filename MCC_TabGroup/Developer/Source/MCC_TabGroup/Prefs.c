#include "MUI/TabGroup_mcc.h"
#include "Common.h"

#define CATCOMP_NUMBERS 1
#include "Locale.h"

#include <libraries/asl.h>
#include <libraries/mui.h>
#include <proto/alib.h>
#include <proto/exec.h>

// Borrow some key MUI features that were not made public...
#ifndef PopimageObject
#define PopimageObject MUI_NewObject("Popimage.mui"
#endif

#ifndef MUIA_Imageadjust_Type
#define MUIA_Imageadjust_Type   0x80422F2BU
#endif

#ifndef MUIA_Imagedisplay_Spec
#define MUIA_Imagedisplay_Spec  0x8042A547U
#endif

#define MUIC_TabGroupP "TabGroup.mcp"
#define CLASS MUIC_TabGroupP

typedef struct Data {
    Object* background_image;
    Object* font_string;
    Object* horiz_pad_slider;
    Object* vert_pad_slider;
    Object* compact_checkmark;
} PrefsData;

#define USE_PREFSIMAGE_BODY
#define USE_PREFSIMAGE_COLORS

const UBYTE PrefsImage_body[];
const ULONG PrefsImage_colors[];

#include "PrefsImage.h"
#include "MCCLib.c"

#include <string.h>

static BOOL ClassInitFunc(const struct Library* const base);
static void ClassExitFunc(const struct Library* const base);
static ULONG Dispatcher(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG prefs_new(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg);
static ULONG prefs_config_to_gadgets(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_Settingsgroup_ConfigToGadgets* msg);
static ULONG prefs_gadgets_to_config(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_Settingsgroup_GadgetsToConfig* msg);

static STRPTR info_text;

static BOOL ClassInitFunc(const struct Library* const base) {
    Locale_Open("TabGroup_mcp.catalog", 1, 0);

    const char* info_text_parts[] = {
        MUIX_C MUIX_B MUIC_TabGroupP MUIX_N " " VERSIONSTR " " __AMIGADATE__ "\n",
        GetString(MSG_Copyright),
        " " COPYRIGHT " " AUTHOR "\n",
        GetString(MSG_Distribution)
    };

    ULONG num_parts = sizeof(info_text_parts) / sizeof(info_text_parts[0]);
    ULONG info_text_size = 1;

    for (ULONG i = 0; i < num_parts; ++ i) {
        info_text_size += strlen(info_text_parts[i]);
    }

    if (! (info_text = AllocVec(info_text_size, MEMF_CLEAR))) {
        return FALSE;
    }

    for (ULONG i = 0; i < num_parts; ++ i) {
        strcat(info_text, info_text_parts[i]);
    }

    return TRUE;
}

static void ClassExitFunc(const struct Library* const base) {
    FreeVec(info_text);

    Locale_Close();
}

static ULONG Dispatcher(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") Msg msg) {
    switch (msg->MethodID) {
    case OM_NEW: return prefs_new(cl, obj, msg);
    case MUIM_Settingsgroup_ConfigToGadgets: return prefs_config_to_gadgets(cl, obj, (struct MUIP_Settingsgroup_ConfigToGadgets*)msg);
    case MUIM_Settingsgroup_GadgetsToConfig: return prefs_gadgets_to_config(cl, obj, (struct MUIP_Settingsgroup_GadgetsToConfig*)msg);
    }

    return DoSuperMethodA(cl, obj, msg);
}

static ULONG prefs_new(__reg("a0") struct IClass* cl, __reg("a2") Object* trueclass, __reg("a1") Msg msg) {
    Object* obj = (Object*)DoSuperMethodA(cl, trueclass, msg);

    if (! obj) {
        return 0;
    }

    PrefsData* data = INST_DATA(cl, obj);
    Object* tab_group;

    Object* group = VGroup,
        Child, VGroup,
            GroupFrameT(GetString(MSG_Sample)),
            Child, tab_group = TabGroupObject,
                MUIA_TabGroup_DraggableTabs, TRUE,
                MUIA_CycleChain, 1,
            End,
        End,
        Child, VGroup,
            GroupFrameT(GetString(MSG_Tab_Settings)),
            MUIA_Group_Columns, 2,
            Child, FreeLabel(GetString(MSG_Background)),
            Child, data->background_image = PopimageObject,
                MUIA_CycleChain, 1,
                MUIA_Draggable, TRUE,
                MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
                MUIA_Window_Title, GetString(MSG_Background_Color),
                MUIA_ShortHelp, GetString(MSG_Background_Help),
            End,
            Child, Label(GetString(MSG_Font)),
            Child, data->font_string = PopaslObject,
                MUIA_CycleChain, 1,
                MUIA_Popstring_String, String(NULL, MAX_FONT_SPEC_SIZE),
                MUIA_Popstring_Button, PopButton(MUII_PopUp),
                MUIA_Popasl_Type, ASL_FontRequest,
                MUIA_ShortHelp, GetString(MSG_Font_Help),
                ASLFO_TitleText, GetString(MSG_Select_Font),
            End,
            Child, Label(GetString(MSG_Horizontal_Padding)),
            Child, data->horiz_pad_slider = SliderObject,
                MUIA_CycleChain, 1,
                MUIA_Slider_Min, 0,
                MUIA_Slider_Max, 10,
                MUIA_ShortHelp, GetString(MSG_Horizontal_Padding_Help),
            End,
            Child, Label(GetString(MSG_Vertical_Padding)),
            Child, data->vert_pad_slider = SliderObject,
                MUIA_CycleChain, 1,
                MUIA_Slider_Min, 0,
                MUIA_Slider_Max, 10,
                MUIA_ShortHelp, GetString(MSG_Vertical_Padding_Help),
            End,
            Child, Label(GetString(MSG_Compact)),
            Child, HGroup,
                MUIA_Group_Spacing, 0,
                Child, data->compact_checkmark = CheckMark(FALSE),
                Child, HSpace(0),
            End,
        End,
        Child, TextObject,
            TextFrame,
            MUIA_Background, MUII_TextBack,
            MUIA_FixHeightTxt, "\n",
            MUIA_Text_Contents, info_text,
        End,
    End;

    if (! group) {
        CoerceMethod(cl, obj, OM_DISPOSE);
        return 0;
    }

    set(data->compact_checkmark, MUIA_ShortHelp, GetString(MSG_Compact_Help));

    for (ULONG page_idx = 1; page_idx <= 3; ++ page_idx) {
        UBYTE page_name[20];
        UBYTE page_text[20];

        static UWORD put_char[] = {0x16C0, 0x4e75}; // move.b d0,(a3)+ ; rts
        RawDoFmt(GetString(MSG_Page), &page_idx, (void (*)())put_char, page_name);
        RawDoFmt(GetString(MSG_Page_Body), &page_idx, (void (*)())put_char, page_text);

        DoMethod(tab_group, MUIM_TabGroup_AddTab, MUIV_TabGroup_Tab_Last, page_name, 0, TRUE,
            VCenter(HCenter(MUI_NewObject(MUIC_Text, MUIA_Text_Contents, page_text, TAG_DONE))), TAG_DONE);
    }

    DoMethod(obj, OM_ADDMEMBER, group);

    return (ULONG)obj;
}

static ULONG prefs_config_to_gadgets(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_Settingsgroup_ConfigToGadgets* msg) {
    PrefsData* data = INST_DATA(cl, obj);

    STRPTR background = "6:27"; // MUII_RegisterBack
    STRPTR font = "";
    ULONG horiz_pad = TAB_DEFAULT_EXTRA_PAD_HORIZ;
    ULONG vert_pad = TAB_DEFAULT_EXTRA_PAD_VERT;
    BOOL compact = TAB_DEFAULT_COMPACT;

    ULONG config;

    if ((config = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TabGroup_Background)) != 0) {
        background = (STRPTR)config;
    }

    if ((config = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TabGroup_Font)) != 0) {
        font = (STRPTR)config;
    }

    if ((config = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TabGroup_Horizontal_Padding)) != 0) {
        horiz_pad = *(ULONG*)config;
    }

    if ((config = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TabGroup_Vertical_Padding)) != 0) {
        vert_pad = *(ULONG*)config;
    }

    if ((config = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_TabGroup_CompactTabs)) != 0) {
        compact = *(ULONG*)config;
    }

    set(data->background_image, MUIA_Imagedisplay_Spec, background);
    set(data->font_string, MUIA_String_Contents, font);
    set(data->horiz_pad_slider, MUIA_Numeric_Value, horiz_pad);
    set(data->vert_pad_slider, MUIA_Numeric_Value, vert_pad);
    set(data->compact_checkmark, MUIA_Selected, compact);

    return 0;
}

static ULONG prefs_gadgets_to_config(__reg("a0") struct IClass* cl, __reg("a2") Object* obj, __reg("a1") struct MUIP_Settingsgroup_GadgetsToConfig* msg) {
    PrefsData* data = INST_DATA(cl, obj);

    STRPTR background;
    STRPTR font;
    ULONG horiz_pad;
    ULONG vert_pad;
    ULONG compact;

    get(data->background_image, MUIA_Imagedisplay_Spec, &background);
    get(data->font_string, MUIA_String_Contents, &font);
    get(data->horiz_pad_slider, MUIA_Numeric_Value, &horiz_pad);
    get(data->vert_pad_slider, MUIA_Numeric_Value, &vert_pad);
    get(data->compact_checkmark, MUIA_Selected, &compact);

    DoMethod(msg->configdata, MUIM_Dataspace_Add, background, strlen(background) + 1, MUICFG_TabGroup_Background);
    DoMethod(msg->configdata, MUIM_Dataspace_Add, font, strlen(font) + 1, MUICFG_TabGroup_Font);
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &horiz_pad, sizeof(ULONG), MUICFG_TabGroup_Horizontal_Padding);
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &vert_pad, sizeof(ULONG), MUICFG_TabGroup_Vertical_Padding);
    DoMethod(msg->configdata, MUIM_Dataspace_Add, &compact, sizeof(ULONG), MUICFG_TabGroup_CompactTabs);

    return 0;
}
