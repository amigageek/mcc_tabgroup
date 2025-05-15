#ifndef LOCALESTRINGS_H
#define LOCALESTRINGS_H 1

/* Locale Catalog Source File
 *
 * Automatically created by SimpleCat V3
 * Do NOT edit by hand!
 *
 * SimpleCat ©1992-2013 Guido Mersmann
 *
 */



/****************************************************************************/


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif



/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_Sample 0
#define MSG_Tab_Settings 1
#define MSG_Background 2
#define MSG_Background_Color 3
#define MSG_Background_Help 4
#define MSG_Font 5
#define MSG_Font_Help 6
#define MSG_Select_Font 7
#define MSG_Horizontal_Padding 8
#define MSG_Horizontal_Padding_Help 9
#define MSG_Vertical_Padding 10
#define MSG_Vertical_Padding_Help 11
#define MSG_Compact 12
#define MSG_Compact_Help 13
#define MSG_Copyright 14
#define MSG_Distribution 15
#define MSG_Page 16
#define MSG_Page_Body 17

#define CATCOMP_LASTID 17

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_Sample_STR "Sample"
#define MSG_Tab_Settings_STR "Tab Settings"
#define MSG_Background_STR "Background:"
#define MSG_Background_Color_STR "Background Color"
#define MSG_Background_Help_STR "Background for pages and selected tab"
#define MSG_Font_STR "Font:"
#define MSG_Font_Help_STR "Font used in tab titles"
#define MSG_Select_Font_STR "Select Font"
#define MSG_Horizontal_Padding_STR "Horizontal Padding:"
#define MSG_Horizontal_Padding_Help_STR "Space to left and right of tab title"
#define MSG_Vertical_Padding_STR "Vertical Padding:"
#define MSG_Vertical_Padding_Help_STR "Space above and below tab title"
#define MSG_Compact_STR "Compact:"
#define MSG_Compact_Help_STR "Tabs expand no wider than necessary"
#define MSG_Copyright_STR "Copyright"
#define MSG_Distribution_STR "Distributed under the MIT-0 license"
#define MSG_Page_STR "Page %lu"
#define MSG_Page_Body_STR "This is page %lu"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x08"
    MSG_Sample_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x0E"
    MSG_Tab_Settings_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x0C"
    MSG_Background_STR "\x00"
    "\x00\x00\x00\x03\x00\x12"
    MSG_Background_Color_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x26"
    MSG_Background_Help_STR "\x00"
    "\x00\x00\x00\x05\x00\x06"
    MSG_Font_STR "\x00"
    "\x00\x00\x00\x06\x00\x18"
    MSG_Font_Help_STR "\x00"
    "\x00\x00\x00\x07\x00\x0C"
    MSG_Select_Font_STR "\x00"
    "\x00\x00\x00\x08\x00\x14"
    MSG_Horizontal_Padding_STR "\x00"
    "\x00\x00\x00\x09\x00\x26"
    MSG_Horizontal_Padding_Help_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x12"
    MSG_Vertical_Padding_STR "\x00"
    "\x00\x00\x00\x0B\x00\x20"
    MSG_Vertical_Padding_Help_STR "\x00"
    "\x00\x00\x00\x0C\x00\x0A"
    MSG_Compact_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x24"
    MSG_Compact_Help_STR "\x00"
    "\x00\x00\x00\x0E\x00\x0A"
    MSG_Copyright_STR "\x00"
    "\x00\x00\x00\x0F\x00\x24"
    MSG_Distribution_STR "\x00"
    "\x00\x00\x00\x10\x00\x0A"
    MSG_Page_STR "\x00\x00"
    "\x00\x00\x00\x11\x00\x12"
    MSG_Page_Body_STR "\x00\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/



#endif /* LOCALESTRINGS_H */

