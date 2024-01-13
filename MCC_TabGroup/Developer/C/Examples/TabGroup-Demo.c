#include <MUI/TabGroup_mcc.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <stdio.h>

void new_tab(void);
void close_tab(void);
void non_paged_tab_changed(__reg("a1") Object** active_tab_p);

struct Hook new_tab_hook = { .h_Entry = (ULONG (*)())new_tab };
struct Hook close_tab_hook = { .h_Entry = (ULONG (*)())close_tab };
struct Hook non_paged_tab_changed_hook = { .h_Entry = (ULONG (*)())non_paged_tab_changed };

Object* tab_group_paged;
Object* tab_group_non_paged;
Object* non_paged_text;

int main() {
    Object* window;
    Object* new_tab_button;
    Object* close_tab_button;

    Object* app = ApplicationObject,
        MUIA_Application_Title, "TabGroup Demo",

        SubWindow, window = WindowObject,
            MUIA_Window_Title, "TabGroup Demo",

            WindowContents, VGroup,
                Child, VGroup,
                    GroupFrameT("Multiple Pages, Draggable"),
                    Child, tab_group_paged = TabGroupObject,
                        MUIA_TabGroup_PageMode, TRUE,
                        MUIA_TabGroup_DraggableTabs, TRUE,
                        MUIA_CycleChain, 1,
                    End,
                End,
                Child, VGroup,
                    GroupFrameT("Single Page, Not Draggable"),
                    Child, tab_group_non_paged = TabGroupObject,
                        MUIA_TabGroup_PageMode, FALSE,
                        MUIA_TabGroup_DraggableTabs, FALSE,
                        MUIA_CycleChain, 1,
                        MUIA_Group_Spacing, 0,
                        Child, VSpace(0),
                        Child, HCenter(non_paged_text = MUI_NewObject(MUIC_Text, TAG_DONE)),
                        Child, VSpace(0),
                    End,
                End,
                Child, HGroup,
                    Child, new_tab_button = SimpleButton("New Tab"),
                    Child, close_tab_button = SimpleButton("Close Tab"),
                End,
            End,
        End,
    End;

    if (! app) {
        MUI_Request(NULL, NULL, 0, "TabGroup Demo", "_Ok", "MUI object creation failed.\nHave you installed TabGroup.mcc?");
        goto exit;
    }

    set(new_tab_button, MUIA_CycleChain, 1);
    set(close_tab_button, MUIA_CycleChain, 1);

    DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(new_tab_button, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_CallHook, &new_tab_hook);
    DoMethod(close_tab_button, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_CallHook, &close_tab_hook);

    // Non-paged TabGroups should react to active tab changes
    DoMethod(tab_group_non_paged, MUIM_Notify, MUIA_TabGroup_ActiveTab, MUIV_EveryTime,
        app, 3, MUIM_CallHook, &non_paged_tab_changed_hook, MUIV_TriggerValue);

    // TabGroups should always have at least one tab to render correctly
    new_tab();

    set(window, MUIA_Window_Open, TRUE);

    for (ULONG signals = 0;; ) {
        if (DoMethod(app, MUIM_Application_NewInput, &signals) == MUIV_Application_ReturnID_Quit) {
            break;
        }

        if (signals) {
            signals = Wait(signals);
        }
    }

exit:
    MUI_DisposeObject(app);

    return 0;
}

void new_tab(void) {
    static ULONG next_page_id = 0;
    ++ next_page_id;

    UBYTE page_name[] = "Page XXX";
    UBYTE page_text[] = "This is page XXX";

    snprintf(page_name, sizeof(page_name), "Page %lu", next_page_id);
    snprintf(page_text, sizeof(page_text), "This is page %lu", next_page_id);

    Object* new_page = VCenter(HCenter(MUI_NewObject(MUIC_Text, MUIA_Text_Contents, page_text, TAG_DONE)));

    DoMethod(tab_group_paged, MUIM_TabGroup_AddTab, MUIV_TabGroup_Tab_Last, page_name, 0, TRUE, new_page);
    DoMethod(tab_group_non_paged, MUIM_TabGroup_AddTab, MUIV_TabGroup_Tab_Last, page_name, next_page_id, TRUE, NULL);
}

void close_tab(void) {
    ULONG num_tabs;
    get(tab_group_paged, MUIA_TabGroup_NumTabs, &num_tabs);

    if (num_tabs < 2) {
        return;
    }

    Object* active_paged_tab;
    Object* active_page;

    get(tab_group_paged, MUIA_TabGroup_ActiveTab, &active_paged_tab);
    get(active_paged_tab, MUIA_TabGroup_Tab_Page, &active_page);

    DoMethod(tab_group_paged, MUIM_TabGroup_RemoveTab, active_paged_tab);
    DoMethod(tab_group_non_paged, MUIM_TabGroup_RemoveTab, MUIV_TabGroup_Tab_Active);

    // When a paged tab is removed ownership of the page returns to the user
    // When the TabGroup is disposed then any attached pages are also disposed
    MUI_DisposeObject(active_page);
}

void non_paged_tab_changed(__reg("a1") Object** active_tab_p) {
    // In a real application you might set UserData to a per-page data model
    // When the active tab changes you would rebind the page controls to this model
    ULONG tab_id;
    get(*active_tab_p, MUIA_TabGroup_Tab_UserData, &tab_id);

    DoMethod(non_paged_text, MUIM_SetAsString, MUIA_Text_Contents, "This is page %lu", tab_id);
}
