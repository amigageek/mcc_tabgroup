/*
**
** Copyright � 1996-1998,1999 Dipl.-Inform. Kai Hofmann. All rights reserved.
**
** $VER: MCCLib.c 12.5 (01.12.99)
**
** Released as Giftware for NON-COMMERCIAL use!
**
** TODO:
** INFOCLASS
** Combined mcc+mcp
** Alternative superclass
*/

 /* ------------------------------------------------------------------------ */
 /*
    Before:
    CLASS	MUIC_MonthNavigator
    SUPERCLASS	MUIC_Group ; DO NOT SPECIFY THIS FOR MCPs!
    ONLYGLOBAL	Must only be set to TRUE for a MCP when it should be global
		available only!
    COPYRIGHT	"1997-1998,1999"
    AUTHOR	"Dipl.-Inform. Kai Hofmann"
    VERSION	12
    REVISION	0
    VERSIONSTR	"12.0"

    PREFSIMAGE_COMPRESSION  0 ; MCPs ONLY!
    PREFSIMAGE_DEPTH	    3 ; MCPs ONLY!
    PREFSIMAGE_MASKING	    2 ; MCPs ONLY!
    PREFSIMAGE_WIDTH	   23 ; MCPs ONLY!
    PREFSIMAGE_HEIGHT	   14 ; MCPs ONLY!

    GCC:
    __AMIGADATE__	"(dd.mm.yy)"

    struct Data {};
    extern const UBYTE PrefsImage_body[];    ; MCPs ONLY!
    extern const ULONG PrefsImage_colors[];  ; MCPs ONLY!

    Somewhere:
    static BOOL ClassInitFunc(const struct Library *const base);
    static VOID ClassExitFunc(const struct Library *const base);
    static ULONG SAVEDS_ASM Dispatcher(REG(A0) struct IClass *const cl GCCREG(A0), REG(A2) Object *const obj GCCREG(A2), REG(A1) const Msg msg GCCREG(A1));
 */
 /* ------------------------------------------------------------------------ */

 #define __USE_SYSBASE 1
 #ifndef PROTO_EXEC_H
   #include <proto/exec.h>
 #endif
 #ifndef EXEC_RESIDENT_H
   #include <exec/resident.h>
 #endif
 #ifndef EXEC_MEMORY_H
   #include <exec/memory.h>
 #endif
 #ifndef EXEC_LIBRARIES_H
   #include <exec/libraries.h>
 #endif
 #ifndef EXEC_SEMAPHORES_H
   #include <exec/semaphores.h>
 #endif
 #ifndef EXEC_EXECBASE_H
   #include <exec/execbase.h>
 #endif

 #ifndef LIBRARIES_MUI_H
   #include <libraries/mui.h>
 #endif
 #ifndef PROTO_MUIMASTER_H
   #include <proto/muimaster.h>
 #endif


 /* copied from system.h � 1995-1997 Dipl.-Inform. Kai Hofmann */
 #ifndef SYSTEM_H
   #define SYSTEM_H

   #if defined(__SASC)
     #define STACKARGS		__stdargs
     #define REGISTERARGS	__regargs
     #define SAVEDS_ASM		__saveds __asm
     #define ALIGNED		__aligned
     #define REG(r)		register r
     #define GCCREG(r)
     #define D0			__d0
     #define D1			__d1
     #define D2			__d2
     #define D3			__d3
     #define D4			__d4
     #define D5			__d5
     #define D6			__d6
     #define D7			__d7
     #define A0			__a0
     #define A1			__a1
     #define A2			__a2
     #define A3			__a3
     #define A4			__a4
     #define A5			__a5
     #define A6			__a6

   #elif defined(__GNUC__)
     #define STACKARGS		__stdargs
     #define REGISTERARGS	__regargs
     #define SAVEDS_ASM 	__saveds
     #define ALIGNED		__aligned
     #define REG(r)		register
     #define GCCREG(r)		__asm(r)
     #define D0			"d0"
     #define D1			"d1"
     #define D2			"d2"
     #define D3			"d3"
     #define D4			"d4"
     #define D5			"d5"
     #define D6			"d6"
     #define D7			"d7"
     #define A0			"a0"
     #define A1			"a1"
     #define A2			"a2"
     #define A3			"a3"
     #define A4			"a4"
     #define A5			"a5"
     #define A6			"a6"

   #elif defined(__STORM__)
     #define STACKARGS
     #define REGISTERARGS
     #define SAVEDS_ASM		__saveds
     #define ALIGNED
     #define REG(r)		register r
     #define GCCREG(r)
     #define D0			__d0
     #define D1			__d1
     #define D2			__d2
     #define D3			__d3
     #define D4			__d4
     #define D5			__d5
     #define D6			__d6
     #define D7			__d7
     #define A0			__a0
     #define A1			__a1
     #define A2			__a2
     #define A3			__a3
     #define A4			__a4
     #define A5			__a5
     #define A6			__a6

   #elif defined(__MAXON__)
     #define STACKARGS
     #define REGISTERARGS
     #define SAVEDS_ASM		__saveds
     #define SAVEDS_ASM
     #define ALIGNED
     #define REG(r)		register r
     #define GCCREG(r)
     #define D0			__d0
     #define D1			__d1
     #define D2			__d2
     #define D3			__d3
     #define D4			__d4
     #define D5			__d5
     #define D6			__d6
     #define D7			__d7
     #define A0			__a0
     #define A1			__a1
     #define A2			__a2
     #define A3			__a3
     #define A4			__a4
     #define A5			__a5
     #define A6			__a6

   #elif defined(_DCC)
     #define STACKARGS		__stkargs
     #define REGISTERARGS	__regargs
     #define SAVEDS_ASM		__geta4 __saveds __asm
     #define ALIGNED		__aligned
     #define REG(r)		register r
     #define GCCREG(r)
     #define D0			__D0
     #define D1			__D1
     #define D2			__D2
     #define D3			__D3
     #define D4			__D4
     #define D5			__D5
     #define D6			__D6
     #define D7			__D7
     #define A0			__A0
     #define A1			__A1
     #define A2			__A2
     #define A3			__A3
     #define A4			__A4
     #define A5			__A5
     #define A6			__A6

   #elif defined(__VBCC__)
     #define STACKARGS
     #define REGISTERARGS
     #define SAVEDS_ASM
     #define ALIGNED
     #define REG(r)		register __reg(r)
     #define GCCREG(r)
     #define D0			"d0"
     #define D1			"d1"
     #define D2			"d2"
     #define D3			"d3"
     #define D4			"d4"
     #define D5			"d5"
     #define D6			"d6"
     #define D7			"d7"
     #define A0			"a0"
     #define A1			"a1"
     #define A2			"a2"
     #define A3			"a3"
     #define A4			"a4"
     #define A5			"a5"
     #define A6			"a6"

     #pragma dontwarn 68
     #pragma dontwarn 79
     #pragma dontwarn 85
     #pragma dontwarn 168
   #elif defined(AZTEC)

   #elif defined(LATTICE)

   #endif
 #endif

 #ifndef __AMIGADATE__
   #ifdef __GNUC__
     #define __AMIGADATE__	"(" __DATE__ ")"
   #endif
   #ifdef __STORM__
     #define __AMIGADATE__	"(" __DATE2__ ")"
   #endif

 #endif


 #ifndef SUPERCLASS
   #define SUPERCLASS	MUIC_Mccprefs
   #define PREFSCLASS	1
   #ifndef ONLYGLOBAL
     #define ONLYGLOBAL FALSE
   #endif
 #endif

 /* ------------------------------------------------------------------------ */

 /****************************************************************************/
 /* External references 						     */
 /****************************************************************************/

 static BOOL SAVEDS_ASM UserLibOpen   (REG(A6) const struct Library *const base GCCREG(A6));
 static BOOL SAVEDS_ASM UserLibClose  (REG(A6) const struct Library *const base GCCREG(A6));
 static BOOL ClassInitFunc(const struct Library *const base);
 static VOID ClassExitFunc(const struct Library *const base);
 static ULONG SAVEDS_ASM Dispatcher(REG(A0) struct IClass *const cl GCCREG(A0), REG(A2) Object *const obj GCCREG(A2), REG(A1) const Msg msg GCCREG(A1));

 struct LibraryHeader * SAVEDS_ASM _LibInit(REG(A0) const BPTR Segment GCCREG(A0));
 BPTR SAVEDS_ASM _LibExpunge(REG(A6) struct LibraryHeader *const base GCCREG(A6));
 struct LibraryHeader * SAVEDS_ASM _LibOpen(REG(A6) struct LibraryHeader *const base GCCREG(A6));
 BPTR SAVEDS_ASM _LibClose(REG(A6) struct LibraryHeader *const base GCCREG(A6));
 LONG SAVEDS_ASM _LibExtFunc(VOID);

 struct MUI_CustomClass * SAVEDS_ASM MCC_Query(REG(D0) const LONG which GCCREG(D0));

 /****************************************************************************/
 /* Dummy entry point and LibExtFunc() function all in one		     */
 /****************************************************************************/

 LONG SAVEDS_ASM _LibExtFunc(VOID)
  {
   return(NULL);
  }

 /****************************************************************************/
 /* Structures								     */
 /****************************************************************************/

 const char ALIGNED _LibName[] = CLASS;
 const char ALIGNED _LibID[] = "$VER: " CLASS " " VERSIONSTR " " __AMIGADATE__ " �" COPYRIGHT " " AUTHOR ", "
			       #if defined(_M68060) || defined(__mc68060) || defined(mc68060)
				 "68060"
			       #elif defined(_M68040) || defined(__mc68040) || defined(mc68040)
				 "68040"
			       #elif defined(_M68030) || defined(__mc68030) || defined(mc68030)
				 "68030"
			       #elif defined(_M68020) || defined(__mc68020) || defined(mc68020)
				 "68020"
			       #elif defined(_M68010) || defined(__mc68010) || defined(mc68010)
				 "68010"
			       #else /* defined(_M68000) || defined(__mc68000) || defined(mc68000) */
				 "68000"
			       #endif
			       #if defined(_MFPU40) /* || defined(__mc68040) || defined(mc68040) */
				 "+FPU40"
			       #elif defined(_M68882)
				 "+68882"
			       #elif defined(_M68881) || defined(__HAVE_68881__)
				 "+68881"
			       #endif
			       " version.\n";

 const APTR ALIGNED _LibFuncTab[] =
  {
   _LibOpen,
   _LibClose,
   _LibExpunge,
   _LibExtFunc,
   MCC_Query,
   (APTR)-1
  };

 const struct Resident ALIGNED _LibRomTag =
  {
   RTC_MATCHWORD,
   (struct Resident *)&_LibRomTag,
   (APTR)(&_LibRomTag + 1),
   0,
   VERSION,
   NT_LIBRARY,
   0,
   (char *)_LibName,
   (char *)_LibID,
   _LibInit
  };


 struct LibraryHeader
  {
   struct Library	  lh_Library;
   BPTR 		  lh_Segment;
   struct SignalSemaphore lh_Semaphore;
  };


 struct Library 	*MUIClassBase = NULL;
 struct Library 	*MUIMasterBase = NULL;
 struct MUI_CustomClass *ThisClass = NULL;
 struct ExecBase	*SysBase = NULL;
 struct IntuitionBase	*IntuitionBase = NULL;
 #if defined(__SASC) || defined(_DCC)
   struct Library	*UtilityBase = NULL;
 #else
   struct UtilityBase 	*UtilityBase = NULL;
 #endif
 struct DosLibrary	*DOSBase = NULL;
 struct GfxBase 	*GfxBase = NULL;

 /****************************************************************************/
 /* Standard Library Functions, all of them are called in Forbid() state.    */
 /****************************************************************************/

 /* REG(D0) struct LibraryHeader *base GCCREG(D0) */
 struct LibraryHeader * SAVEDS_ASM _LibInit(REG(A0) const BPTR Segment GCCREG(A0))
  {
   struct LibraryHeader *base;

   SysBase = *((struct ExecBase **)4);

   #if defined(_M68060) || defined(__mc68060) || defined(mc68060)
   if (!(SysBase->AttnFlags & AFF_68060))
    {
     return(NULL);
    }
   #elif defined(_M68040) || defined(__mc68040) || defined(mc68040)
   if (!(SysBase->AttnFlags & AFF_68040))
    {
     return(NULL);
    }
   #elif defined(_M68030) || defined(__mc68030) || defined(mc68030)
   if (!(SysBase->AttnFlags & AFF_68030))
    {
     return(NULL);
    }
   #elif defined(_M68020) || defined(__mc68020) || defined(mc68020)
   if (!(SysBase->AttnFlags & AFF_68020))
    {
     return(NULL);
    }
   #elif defined(_M68010) || defined(__mc68010) || defined(mc68010)
   if (!(SysBase->AttnFlags & AFF_68010))
    {
     return(NULL);
    }
   #endif
   #if defined(_MFPU40) /* || defined(__mc68040) || defined(mc68040) */
   if (!(SysBase->AttnFlags & AFF_FPU40))
    {
     return(NULL);
    }
   #elif defined(_M68882)
   if (!(SysBase->AttnFlags & AFF_68882))
    {
     return(NULL);
    }
   #elif defined(_M68881) || defined(__HAVE_68881__)
   if (!(SysBase->AttnFlags & AFF_68881))
    {
     return(NULL);
    }
   #endif

   base = (struct LibraryHeader *)MakeLibrary((APTR)_LibFuncTab,NULL,NULL,sizeof(struct LibraryHeader),NULL);
   if (base != NULL)
    {
     base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
     base->lh_Library.lib_Node.ln_Name = (char *)_LibName;
     base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
     base->lh_Library.lib_Version      = VERSION;
     base->lh_Library.lib_Revision     = REVISION;
     base->lh_Library.lib_IdString     = (char *)_LibID;
     base->lh_Segment = Segment;
     InitSemaphore(&base->lh_Semaphore);
     AddLibrary((struct Library *)base);
    }
   return(base);
  }


 BPTR SAVEDS_ASM _LibExpunge(REG(A6) struct LibraryHeader *const base GCCREG(A6))
  {
   struct ExecBase *SysBase = *((struct ExecBase **)4);
   BPTR rc;

   if (base->lh_Library.lib_OpenCnt != 0)
    {
     base->lh_Library.lib_Flags |= LIBF_DELEXP;
     return(NULL);
    }
   Remove((struct Node *)base);
   rc = base->lh_Segment;
   FreeMem((BYTE *)base - base->lh_Library.lib_NegSize,(ULONG)(base->lh_Library.lib_NegSize + base->lh_Library.lib_PosSize));
   return(rc);
  }

 /****************************************************************************/

 /* REG(D0) ULONG Version GCCREG(D0) */
 struct LibraryHeader * SAVEDS_ASM _LibOpen(REG(A6) struct LibraryHeader *const base GCCREG(A6))
  {
   struct ExecBase *SysBase = *((struct ExecBase **)4);
   struct LibraryHeader *rc;

   ObtainSemaphore(&base->lh_Semaphore);
   base->lh_Library.lib_OpenCnt++;
   base->lh_Library.lib_Flags &= ~LIBF_DELEXP;
   if (UserLibOpen((struct Library *)base))
    {
     rc = base;
    }
   else
    {
     rc = NULL;
     base->lh_Library.lib_OpenCnt--;
    }
   ReleaseSemaphore(&base->lh_Semaphore);
   return(rc);
  }


 BPTR SAVEDS_ASM _LibClose(REG(A6) struct LibraryHeader *const base GCCREG(A6))
  {
   struct ExecBase *SysBase = *((struct ExecBase **)4);
   BPTR rc = NULL;
   BOOL exp;

   ObtainSemaphore(&base->lh_Semaphore);
   UserLibClose((struct Library *)base);
   base->lh_Library.lib_OpenCnt--;
   exp = (base->lh_Library.lib_OpenCnt == 0);
   ReleaseSemaphore(&base->lh_Semaphore);
   if (exp && (base->lh_Library.lib_Flags & LIBF_DELEXP))
    {
     rc = _LibExpunge(base);
    }
   return(rc);
  }

 /****************************************************************************/

 static BOOL SAVEDS_ASM UserLibOpen(REG(A6) const struct Library *const base GCCREG(A6))
  {
   if (base->lib_OpenCnt == 1)
    {
     MUIMasterBase = OpenLibrary(MUIMASTER_NAME,(ULONG)base->lib_Version);
     if (MUIMasterBase != NULL)
      {
       ThisClass = MUI_CreateCustomClass((struct Library *const)base,SUPERCLASS,NULL,sizeof(struct Data),Dispatcher);
       if (ThisClass != NULL)
	{
	 BOOL res;

	 UtilityBase   =
                         #if !defined(__SASC) && !defined(_DCC)
                         (struct UtilityBase *)
                         #endif
                         ThisClass->mcc_UtilityBase;
	 DOSBase       = (struct DosLibrary *)ThisClass->mcc_DOSBase;
	 GfxBase       = (struct GfxBase *)ThisClass->mcc_GfxBase;
	 IntuitionBase = (struct IntuitionBase *)ThisClass->mcc_IntuitionBase;
	 res = ClassInitFunc(base);
	 if (res)
	  {
	   return(TRUE);
	  }
	 if (!MUI_DeleteCustomClass(ThisClass))
	  {
	   /* can't delete public custom class */
	  }
	}
       CloseLibrary(MUIMasterBase);
      }
     ThisClass	   = NULL;
     MUIMasterBase = NULL;
     IntuitionBase = NULL;
     UtilityBase   = NULL;
     DOSBase	   = NULL;
     GfxBase	   = NULL;
     return(FALSE);
    }
   return(TRUE);
  }


 static BOOL SAVEDS_ASM UserLibClose(REG(A6) const struct Library *const base GCCREG(A6))
  {
   if (base->lib_OpenCnt == 1)
    {
     ClassExitFunc(base);
     if (ThisClass != NULL)
      {
       if (!MUI_DeleteCustomClass(ThisClass))
	{
	 /* can't delete public custom class */
	}
      }
     if (MUIMasterBase != NULL)
      {
       CloseLibrary(MUIMasterBase);
      }
     ThisClass	     = NULL;
     MUIMasterBase   = NULL;
     IntuitionBase   = NULL;
     UtilityBase     = NULL;
     DOSBase	     = NULL;
     GfxBase	     = NULL;
    }
   return(TRUE);
  }

 /****************************************************************************/

 struct MUI_CustomClass * SAVEDS_ASM MCC_Query(REG(D0) const LONG which GCCREG(D0))
  {
   switch (which)
    {
     #ifndef PREFSCLASS
     case 0 : return(ThisClass);	/* Pointer to Custom Class */
     #else
     case 1 : return(ThisClass);	/* Pointer to Preference Class */
     case 2 : return(
		     #ifndef PREFSIMAGE_DEPTH
		     NULL
		     #else
		     (struct MUI_CustomClass *)
		     MUI_NewObject(MUIC_Bodychunk,
				     MUIA_Bodychunk_Body,	 PrefsImage_body,
				     MUIA_Bodychunk_Compression, PREFSIMAGE_COMPRESSION,
				     MUIA_Bodychunk_Depth,	 PREFSIMAGE_DEPTH,
				     MUIA_Bodychunk_Masking,	 PREFSIMAGE_MASKING,
				     MUIA_Bitmap_Width, 	 PREFSIMAGE_WIDTH,
				     MUIA_Bitmap_Height,	 PREFSIMAGE_HEIGHT,
				     MUIA_FixWidth,		 PREFSIMAGE_WIDTH,
				     MUIA_FixHeight,		 PREFSIMAGE_HEIGHT,
				     MUIA_Bitmap_SourceColors,	 PrefsImage_colors,
				     MUIA_Bitmap_Transparent,	 0,
				   TAG_DONE
				  )
		     #endif
		    );			/* Pointer to Preference Image */
     case 3 : return(ONLYGLOBAL);	/* Only Global */
     #endif
    }
   return(NULL);
  }

 /* ------------------------------------------------------------------------ */

 /* Disable CTRL-C break handling */
 void REGISTERARGS __chkabort(void)
  {}

 void REGISTERARGS _CXBRK(void)
  {}

 /* ------------------------------------------------------------------------ */

#ifdef __VBCC__
#pragma popwarn
#pragma popwarn
#pragma popwarn
#pragma popwarn
#endif
