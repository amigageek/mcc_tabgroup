/****** TabGroup.mui/MUIA_TabGroup_ActiveTab **************************
*
*   NAME
*	MUIA_TabGroup_ActiveTab -- (V19) [.SG], Object *
*
*   SPECIAL INPUTS
*	MUIV_TabGroup_Tab_First
*	MUIV_TabGroup_Tab_Last
*	MUIV_TabGroup_Tab_Prev
*	MUIV_TabGroup_Tab_Next
*	MUIV_TabGroup_Tab_Advance
*
*   FUNCTION
*	Set (or get) the Tab object which is currently selected.
*	The Tab class is private but exposes the attributes listed
*	in the SEE ALSO section below.
*
*   SEE ALSO
*	MUIA_TabGroup_Tab_Active, MUIA_TabGroup_Tab_Index,
*	MUIA_TabGroup_Tab_Page, MUIA_TabGroup_Tab_TabGroup,
*	MUIA_TabGroup_Tab_Title, MUIA_TabGroup_Tab_UserData
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_DraggableTabs **********************
*
*   NAME
*	MUIA_TabGroup_DraggableTabs -- (V19) [I..], BOOL
*
*   FUNCTION
*	Allows the user to reorder tabs by drag-and-drop. This has
*	no functional consequence because the TabGroup maintains the
*	association of pages to tabs. It is just a user convenience.
*
*	MUIA_TabGroup_DraggableTabs defaults to FALSE.
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_NumTabs ****************************
*
*   NAME
*	MUIA_TabGroup_NumTabs -- (V19) [..G], ULONG
*
*   FUNCTION
*	Get the number of tabs currently in the group.
*
*   SEE ALSO
*	MUIA_TabGroup_TabList
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_PageMode ***************************
*
*   NAME
*	MUIA_TabGroup_PageMode -- (V19) [I..], BOOL
*
*   FUNCTION
*	The TabGroup equivalent of MUIA_Group_PageMode. If set to TRUE
*	each tab will display a different page, specified at the time
*	of tab creation.
*
*	If set to FALSE the user should add at least one child to the
*	group, which will be displayed on all tabs. No page object
*	should be specified when creating a new tab.
*
*	MUIA_TabGroup_PageMode defaults to TRUE.
*
*   SEE ALSO
*	MUIA_Group_PageMode
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_Tab_Active *************************
*
*   NAME
*	MUIA_TabGroup_Tab_Active -- (V19) [..G], BOOL
*
*   FUNCTION
*	Get the selected/deselected state of a tab. Only one tab will
*	be selected at any given time.
*
*   NOTES
*	Attribute of the private Tab class, not the TabGroup!
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_Tab_Index **************************
*
*   NAME
*	MUIA_TabGroup_Tab_Index -- (V19) [..G], ULONG
*
*   FUNCTION
*	Get the ordinal 0..N position of the tab in the group of tabs.
*	If MUIA_TabGroup_DraggableTabs was initialized to TRUE then
*	this ordering can be changed by the user.
*
*	This is mainly for internal use. Use MUIA_TabGroup_Tab_UserData
*	to associate order-independent data with a tab.
*
*   NOTES
*	Attribute of the private Tab class, not the TabGroup!
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_Tab_Page ***************************
*
*   NAME
*	MUIA_TabGroup_Tab_Page -- (V19) [..G], Object *
*
*   FUNCTION
*	Get the page object specified during tab creation.
*	If MUIA_TabGroup_PageMode was initialized to FALSE then
*	this will return NULL.
*
*   NOTES
*	Attribute of the private Tab class, not the TabGroup!
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_Tab_TabGroup ***********************
*
*   NAME
*	MUIA_TabGroup_Tab_Page -- (V19) [..G], Object *
*
*   FUNCTION
*	Get the parent TabGroup object to which this tab belongs.
*
*   NOTES
*	Attribute of the private Tab class, not the TabGroup!
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_Tab_Title **************************
*
*   NAME
*	MUIA_TabGroup_Tab_Title -- (V19) [.SG], STRPTR
*
*   FUNCTION
*	Set (or get) the text contents of the tab. The string will be
*	copied internally so you may free the original string.
*
*   NOTES
*	Attribute of the private Tab class, not the TabGroup!
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_Tab_UserData ***********************
*
*   NAME
*	MUIA_TabGroup_Tab_UserData -- (V19) [.SG], ULONG
*
*   FUNCTION
*	Set (or get) user-defined data associated with the tab. This
*	may be an integer, a pointer, or anything that fits.
*
*   NOTES
*	Attribute of the private Tab class, not the TabGroup!
*
**********************************************************************/

/****** TabGroup.mui/MUIA_TabGroup_TabList ****************************
*
*   NAME
*	MUIA_TabGroup_TabList -- (V19) [..G], struct List *
*
*   FUNCTION
*	Get a read-only list of the private Tab objects managed by
*	the TabGroup. This may be traversed with NextObject(). The
*	order of tabs may change if MUIA_TabGroup_DraggableTabs was
*	initialized to TRUE.
*
*	The Tab class is private but exposes the attributes listed in
*	the SEE ALSO section below.
*
*   SEE ALSO
*	MUIA_TabGroup_Tab_Active, MUIA_TabGroup_Tab_Index,
*	MUIA_TabGroup_Tab_Page, MUIA_TabGroup_Tab_TabGroup,
*	MUIA_TabGroup_Tab_Title, MUIA_TabGroup_Tab_UserData
*
**********************************************************************/

/****** TabGroup.mui/MUIM_TabGroup_AddTab ****************************
*
*   NAME
*	MUIM_TabGroup_AddTab -- (V19)
*
*   SYNOPSIS
*	DoMethod(obj, MUIM_TabGroup_AddTab, LONG index, STRPTR title, ULONG user_data, ULONG active, Object *page)
*
*   FUNCTION
*	Add a new tab to the TabGroup.
*
*	If a page object is provided then the TabGroup takes ownership
*	and will dispose of it when the TabGroup is disposed.
*
*   INPUTS
*	index     - Ordinal index 0..N at which to insert the tab
*	            or one of the MUIV_TabGroup_Tab_* special values.
*
*	title     - Initial text contents of the tab.
*
*	user_data - Initial user data associated with the tab.
*
*	active    - Whether the tab should be initially selected.
*
*	page      - The object to display when this tab is selected,
*	            or NULL if MUIA_TabGroup_PageMode is FALSE.
*
*   RESULT
*	The new private Tab object or NULL on failure.
*	See Tab class attributes in the SEE ALSO section below.
*
*   SEE ALSO
*	MUIA_TabGroup_Tab_Active, MUIA_TabGroup_Tab_Index,
*	MUIA_TabGroup_Tab_Page, MUIA_TabGroup_Tab_TabGroup,
*	MUIA_TabGroup_Tab_Title, MUIA_TabGroup_Tab_UserData,
*	MUIM_TabGroup_RemoveTab
*
**********************************************************************/

/****** TabGroup.mui/MUIM_TabGroup_RemoveTab **************************
*
*   NAME
*	MUIM_TabGroup_RemoveTab -- (V19)
*
*   SYNOPSIS
*	DoMethod(obj, MUIM_TabGroup_RemoveTab, Object *tab)
*
*   FUNCTION
*	Remove a tab from the TabGroup.
*
*	If a page object was provided to AddTab then ownership returns
*	to the application. It is then the programmer's responsibility
*	to dispose of it.
*
*   INPUTS
*	tab - The private Tab object returned by AddTab or retrieved
*	      through MUIA_TabGroup_ActiveTab or MUIA_TabGroup_TabList.
*
*   SEE ALSO
*	MUIM_TabGroup_AddTab
*
**********************************************************************/
