# Microsoft Developer Studio Generated NMAKE File, Based on UnInstall.dsp
!IF "$(CFG)" == ""
CFG=UnInstall - Win32 Debug
!MESSAGE No configuration specified. Defaulting to UnInstall - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UnInstall - Win32 Release" && "$(CFG)" != "UnInstall - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnInstall.mak" CFG="UnInstall - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnInstall - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UnInstall - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "UnInstall - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\UnInstall.exe"


CLEAN :
	-@erase "$(INTDIR)\UnInstall.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\UnInstall.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX- /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\UnInstall.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnInstall.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\UnInstall.pdb" /machine:I386 /out:"$(OUTDIR)\UnInstall.exe" /ALIGN:4096 
LINK32_OBJS= \
	"$(INTDIR)\UnInstall.obj"

"$(OUTDIR)\UnInstall.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UnInstall - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\UnInstall.exe" "$(OUTDIR)\UnInstall.bsc"


CLEAN :
	-@erase "$(INTDIR)\UnInstall.obj"
	-@erase "$(INTDIR)\UnInstall.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnInstall.bsc"
	-@erase "$(OUTDIR)\UnInstall.exe"
	-@erase "$(OUTDIR)\UnInstall.ilk"
	-@erase "$(OUTDIR)\UnInstall.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX- /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\UnInstall.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnInstall.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\UnInstall.sbr"

"$(OUTDIR)\UnInstall.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\UnInstall.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnInstall.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\UnInstall.obj"

"$(OUTDIR)\UnInstall.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("UnInstall.dep")
!INCLUDE "UnInstall.dep"
!ELSE 
!MESSAGE Warning: cannot find "UnInstall.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "UnInstall - Win32 Release" || "$(CFG)" == "UnInstall - Win32 Debug"
SOURCE=.\UnInstall.cpp

!IF  "$(CFG)" == "UnInstall - Win32 Release"


"$(INTDIR)\UnInstall.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "UnInstall - Win32 Debug"


"$(INTDIR)\UnInstall.obj"	"$(INTDIR)\UnInstall.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

