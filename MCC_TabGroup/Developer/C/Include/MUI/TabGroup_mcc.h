#ifndef TABGROUP_MCC_H
#define TABGROUP_MCC_H

#include <libraries/mui.h>

#define MUIC_TabGroup "TabGroup.mcc"
#define TabGroupObject MUI_NewObject(MUIC_TabGroup

/* Methods */
#define MUIM_TabGroup_AddTab        0xC44A0001U /* V19 */
#define MUIM_TabGroup_RemoveTab     0xC44A0002U /* V19 */
struct MUIP_TabGroup_AddTab         { ULONG MethodID; LONG index; STRPTR title; ULONG user_data; ULONG active; Object *page; };
struct MUIP_TabGroup_RemoveTab      { ULONG MethodID; Object *tab; };

/* Attributes */
#define MUIA_TabGroup_PageMode      0xC44A0003U /* V19 i.. BOOL          */
#define MUIA_TabGroup_DraggableTabs 0xC44A0004U /* V19 i.. BOOL          */
#define MUIA_TabGroup_ActiveTab     0xC44A0005U /* V19 .sg Object *      */
#define MUIA_TabGroup_NumTabs       0xC44A0006U /* V19 ..g ULONG         */
#define MUIA_TabGroup_TabList       0xC44A0007U /* V19 ..g struct List * */

/* Special values */
#define MUIV_TabGroup_Tab_First      0          /* V19 */
#define MUIV_TabGroup_Tab_Last      -1          /* V19 */
#define MUIV_TabGroup_Tab_Prev      -2          /* V19 */
#define MUIV_TabGroup_Tab_Next      -3          /* V19 */
#define MUIV_TabGroup_Tab_Advance   -4          /* V19 */
#define MUIV_TabGroup_Tab_Active    -5          /* V19 */

/* Attributes of private Tab class */
#define MUIA_TabGroup_Tab_TabGroup  0xC44A0008U /* V19 ..g Object *      */
#define MUIA_TabGroup_Tab_Title     0xC44A0009U /* V19 .sg STRPTR        */
#define MUIA_TabGroup_Tab_UserData  0xC44A000AU /* V19 .sg ULONG         */
#define MUIA_TabGroup_Tab_Page      0xC44A000BU /* V19 ..g Object *      */
#define MUIA_TabGroup_Tab_Active    0xC44A000CU /* V19 ..g BOOL          */
#define MUIA_TabGroup_Tab_Index     0xC44A000DU /* V19 ..g ULONG         */

#endif
