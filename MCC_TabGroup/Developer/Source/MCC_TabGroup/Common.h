#include <libraries/mui.h>

#define SWAP(T, A, B) { T temp = A; A = B; B = temp; }

#define VERSION 19
#define REVISION 1
#define VERSIONSTR "19.1"
#define COPYRIGHT "2025"
#define AUTHOR "djay@amigageek.com"

#define MAX_FONT_SPEC_SIZE 64
#define TAB_DEFAULT_EXTRA_PAD_HORIZ 2
#define TAB_DEFAULT_EXTRA_PAD_VERT 0
#define TAB_DEFAULT_COMPACT FALSE

#define MUIM_TabGroup_SwapTabs              0xC44A000EU
#define MUICFG_TabGroup_Background          0xC44A000FU
#define MUICFG_TabGroup_Font                0xC44A0010U
#define MUICFG_TabGroup_Horizontal_Padding  0xC44A0011U
#define MUICFG_TabGroup_Vertical_Padding    0xC44A0012U
#define MUICFG_TabGroup_CompactTabs         0xC44A0013U

struct MUIP_TabGroup_SwapTabs { ULONG MethodID; Object* tab1; Object* tab2; };

struct MUI_CustomClass* tab_make_class(void);
void insert_member(Object* parent, struct List* member_list, Object* new_member, UWORD insert_index);
Object* nth_object(struct List* list, UWORD object_index);
ULONG DoSuperNew(struct IClass* cl, Object* obj, ULONG tag1, ...);
void draw_background(Object* instance, LONG left, LONG top, LONG width, LONG height);
