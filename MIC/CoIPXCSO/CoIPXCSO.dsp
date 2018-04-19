# Microsoft Developer Studio Project File - Name="CoIPXCSO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CoIPXCSO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CoIPXCSO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CoIPXCSO.mak" CFG="CoIPXCSO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CoIPXCSO - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CoIPXCSO - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/MIC 2.000", GJSAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CoIPXCSO - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /I "../CommonSource" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib mcbcio32/Mcbcio32.lib DelayImp.lib ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /DelayLoad:McbCIO32.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\CoIPXCSO.dll
InputPath=.\Debug\CoIPXCSO.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CoIPXCSO - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "../CommonSource" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib mcbcio32/Mcbcio32.lib DelayImp.lib ws2_32.lib /nologo /subsystem:windows /dll /machine:I386 /DelayLoad:McbCIO32.dll
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\CoIPXCSO.dll
InputPath=.\ReleaseMinDependency\CoIPXCSO.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "CoIPXCSO - Win32 Debug"
# Name "CoIPXCSO - Win32 Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\CommonSource\ccbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\cmca.cpp
# End Source File
# Begin Source File

SOURCE=.\cmcb.cpp
# End Source File
# Begin Source File

SOURCE=.\CoIPXCSO.cpp

!IF  "$(CFG)" == "CoIPXCSO - Win32 Debug"

# ADD CPP /I "..\CommonSource"

!ELSEIF  "$(CFG)" == "CoIPXCSO - Win32 Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CoIPXCSO.def
# End Source File
# Begin Source File

SOURCE=.\CoIPXCSO.idl
# ADD MTL /tlb ".\CoIPXCSO.tlb" /h "CoIPXCSO.h" /iid "CoIPXCSO_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\CoIPXCSO.rc
# End Source File
# Begin Source File

SOURCE=..\CommonSource\COM_Utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\ComORTEC.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\CSO.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\GamNew.cpp
# End Source File
# Begin Source File

SOURCE=.\IPXCSO.cpp
# End Source File
# Begin Source File

SOURCE=.\NewIPXDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ORTECMcb.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\ORTECSpectrum.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\RegisterByCategory.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TimedMessageDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonSource\VERSION.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cmca.h
# End Source File
# Begin Source File

SOURCE=.\cmcb.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\COM_Utilities.h
# End Source File
# Begin Source File

SOURCE=.\ComORTEC.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\CSO.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\CSO_definitions.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\GamNew.h
# End Source File
# Begin Source File

SOURCE=.\IPXCSO.h
# End Source File
# Begin Source File

SOURCE=.\NewIPXDialog.h
# End Source File
# Begin Source File

SOURCE=.\ORTECMcb.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\ORTECSpectrum.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\RegisterByCategory.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TimedMessageDialog.h
# End Source File
# Begin Source File

SOURCE=..\CommonSource\VERSION.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\IPXCSO.rgs
# End Source File
# End Group
# End Target
# End Project
