# Microsoft Developer Studio Project File - Name="srpc_lua" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=srpc_lua - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "srpc_lua.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "srpc_lua.mak" CFG="srpc_lua - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "srpc_lua - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "srpc_lua - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "srpc_lua - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "srpc_lua___Win32_Release"
# PROP BASE Intermediate_Dir "srpc_lua___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "srpc_lua___Win32_Release"
# PROP Intermediate_Dir "srpc_lua___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./lua" /I "./" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "srpc_lua - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "srpc_lua___Win32_Debug"
# PROP BASE Intermediate_Dir "srpc_lua___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "srpc_lua___Win32_Debug"
# PROP Intermediate_Dir "srpc_lua___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "./lua" /I "./" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "srpc_lua - Win32 Release"
# Name "srpc_lua - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "lua"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lua\lapi.c
# End Source File
# Begin Source File

SOURCE=.\lua\lauxlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lbaselib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lcode.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldblib.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldebug.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldo.c
# End Source File
# Begin Source File

SOURCE=.\lua\ldump.c
# End Source File
# Begin Source File

SOURCE=.\lua\lfunc.c
# End Source File
# Begin Source File

SOURCE=.\lua\lgc.c
# End Source File
# Begin Source File

SOURCE=.\lua\linit.c
# End Source File
# Begin Source File

SOURCE=.\lua\liolib.c
# End Source File
# Begin Source File

SOURCE=.\lua\llex.c
# End Source File
# Begin Source File

SOURCE=.\lua\lmathlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lmem.c
# End Source File
# Begin Source File

SOURCE=.\lua\loadlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lobject.c
# End Source File
# Begin Source File

SOURCE=.\lua\lopcodes.c
# End Source File
# Begin Source File

SOURCE=.\lua\loslib.c
# End Source File
# Begin Source File

SOURCE=.\lua\lparser.c
# End Source File
# Begin Source File

SOURCE=.\lua\lstate.c
# End Source File
# Begin Source File

SOURCE=.\lua\lstring.c
# End Source File
# Begin Source File

SOURCE=.\lua\lstrlib.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltable.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltablib.c
# End Source File
# Begin Source File

SOURCE=.\lua\ltm.c
# End Source File
# Begin Source File

SOURCE=.\lua\lundump.c
# End Source File
# Begin Source File

SOURCE=.\lua\lvm.c
# End Source File
# Begin Source File

SOURCE=.\lua\lzio.c
# End Source File
# Begin Source File

SOURCE=.\lua\print.c
# End Source File
# End Group
# Begin Group "llp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\llp\lib_al.c
# End Source File
# Begin Source File

SOURCE=.\llp\lib_io.c
# End Source File
# Begin Source File

SOURCE=.\llp\lib_lp.c
# End Source File
# Begin Source File

SOURCE=.\llp\lib_mes.c
# End Source File
# Begin Source File

SOURCE=.\llp\lib_table.c
# End Source File
# Begin Source File

SOURCE=.\llp\lp_conf.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\lrpc.c
# End Source File
# Begin Source File

SOURCE=.\lrpcClib.c
# End Source File
# Begin Source File

SOURCE=.\lrpcSlib.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "lua_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lua\lapi.h
# End Source File
# Begin Source File

SOURCE=.\lua\lauxlib.h
# End Source File
# Begin Source File

SOURCE=.\lua\lcode.h
# End Source File
# Begin Source File

SOURCE=.\lua\ldebug.h
# End Source File
# Begin Source File

SOURCE=.\lua\ldo.h
# End Source File
# Begin Source File

SOURCE=.\lua\lfunc.h
# End Source File
# Begin Source File

SOURCE=.\lua\lgc.h
# End Source File
# Begin Source File

SOURCE=.\lua\llex.h
# End Source File
# Begin Source File

SOURCE=.\lua\llimits.h
# End Source File
# Begin Source File

SOURCE=.\lua\lmem.h
# End Source File
# Begin Source File

SOURCE=.\lua\lobject.h
# End Source File
# Begin Source File

SOURCE=.\lua\lopcodes.h
# End Source File
# Begin Source File

SOURCE=.\lua\lparser.h
# End Source File
# Begin Source File

SOURCE=.\lua\lstate.h
# End Source File
# Begin Source File

SOURCE=.\lua\lstring.h
# End Source File
# Begin Source File

SOURCE=.\lua\ltable.h
# End Source File
# Begin Source File

SOURCE=.\lua\ltm.h
# End Source File
# Begin Source File

SOURCE=.\lua\lua.h
# End Source File
# Begin Source File

SOURCE=.\lua\luaconf.h
# End Source File
# Begin Source File

SOURCE=.\lua\lualib.h
# End Source File
# Begin Source File

SOURCE=.\lua\lundump.h
# End Source File
# Begin Source File

SOURCE=.\lua\lvm.h
# End Source File
# Begin Source File

SOURCE=.\lua\lzio.h
# End Source File
# End Group
# Begin Group "llp_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\llp\lib_al.h
# End Source File
# Begin Source File

SOURCE=.\llp\lib_io.h
# End Source File
# Begin Source File

SOURCE=.\llp\lib_lp.h
# End Source File
# Begin Source File

SOURCE=.\llp\lib_mes.h
# End Source File
# Begin Source File

SOURCE=.\llp\lib_table.h
# End Source File
# Begin Source File

SOURCE=.\llp\llp.h
# End Source File
# Begin Source File

SOURCE=.\llp\lp_conf.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lrpc.h
# End Source File
# Begin Source File

SOURCE=.\lrpcClib.h
# End Source File
# Begin Source File

SOURCE=.\lrpcSlib.h
# End Source File
# End Group
# End Target
# End Project
