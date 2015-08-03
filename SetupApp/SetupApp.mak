# Microsoft Developer Studio Generated NMAKE File, Based on SetupApp.dsp
!IF "$(CFG)" == ""
CFG=SetupApp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to SetupApp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SetupApp - Win32 Release" && "$(CFG)" != "SetupApp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SetupApp.mak" CFG="SetupApp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SetupApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SetupApp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SetupApp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ExifPro Install.exe"


CLEAN :
	-@erase "$(INTDIR)\SetupApp.obj"
	-@erase "$(INTDIR)\SetupApp.pch"
	-@erase "$(INTDIR)\SetupApp.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ExifPro Install.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\SetupApp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SetupApp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SetupApp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib COMCTL32.LIB advapi32.lib shell32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ExifPro Install.pdb" /machine:I386 /out:"$(OUTDIR)\ExifPro Install.exe" /ALIGN:4096 
LINK32_OBJS= \
	"$(INTDIR)\SetupApp.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SetupApp.res"

"$(OUTDIR)\ExifPro Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SetupApp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ExifPro Install.exe" "$(OUTDIR)\SetupApp.bsc"


CLEAN :
	-@erase "$(INTDIR)\SetupApp.obj"
	-@erase "$(INTDIR)\SetupApp.pch"
	-@erase "$(INTDIR)\SetupApp.res"
	-@erase "$(INTDIR)\SetupApp.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ExifPro Install.exe"
	-@erase "$(OUTDIR)\ExifPro Install.ilk"
	-@erase "$(OUTDIR)\ExifPro Install.pdb"
	-@erase "$(OUTDIR)\SetupApp.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SetupApp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SetupApp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SetupApp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\SetupApp.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\SetupApp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ExifPro Install.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ExifPro Install.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\SetupApp.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SetupApp.res"

"$(OUTDIR)\ExifPro Install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("SetupApp.dep")
!INCLUDE "SetupApp.dep"
!ELSE 
!MESSAGE Warning: cannot find "SetupApp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SetupApp - Win32 Release" || "$(CFG)" == "SetupApp - Win32 Debug"
SOURCE=.\SetupApp.cpp

!IF  "$(CFG)" == "SetupApp - Win32 Release"


"$(INTDIR)\SetupApp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SetupApp.pch"


!ELSEIF  "$(CFG)" == "SetupApp - Win32 Debug"


"$(INTDIR)\SetupApp.obj"	"$(INTDIR)\SetupApp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\SetupApp.pch"


!ENDIF 

SOURCE=.\SetupApp.rc

"$(INTDIR)\SetupApp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "SetupApp - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\SetupApp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\SetupApp.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "SetupApp - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\SetupApp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\SetupApp.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

