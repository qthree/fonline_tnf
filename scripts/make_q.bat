@echo off

@: Environment
@set PATH=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;%PATH%
@set PATH=C:\Program Files\Microsoft Visual Studio 9.0\VC\BIN;%PATH%
@set LIB=C:\Program Files\Microsoft Visual Studio 9.0\VC\LIB;%LIB%
@set LIB=C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib;%LIB%
@set LIB=.\StlPort;%LIB%
@set INCLUDE=C:\Program Files\Microsoft Visual Studio 9.0\VC\include;%INCLUDE%
@set INCLUDE=.\StlPort;%INCLUDE%

@: Server
@del ".\\fonline_q.dll"
cl.exe /nologo /MT /W3 /O2 /Gd /D "__SERVER" /Fo".\\" /Fd".\\fonline_q.obj" /FD /c ".\\fonline_q.cpp"
link.exe /nologo /dll /incremental:no /machine:I386 ".\\fonline_q.obj" /out:".\\fonline_q.dll"

@: Client
@del ".\\fonline_q_client.dll"
cl.exe /nologo /MT /W3 /O2 /Gd /D "__CLIENT" /Fo".\\fonline_q_client.obj" /Fd".\\" /FD /c ".\\fonline_q.cpp"
link.exe /nologo /dll /incremental:no /machine:I386 ".\\fonline_q_client.obj" /out:".\\fonline_q_client.dll"
@copy ".\\fonline_q_client.dll" "..\\..\\cl_rp_sdk\\data\\cache\\localhost.4001\\fonline_q_client.dll"

@: Delete unnecessary stuff
@del ".\\fonline_q.obj"
@del ".\\fonline_q.exp"
@del ".\\fonline_q.lib"
@del ".\\fonline_q.idb"
@del ".\\fonline_q_client.obj"
@del ".\\fonline_q_client.exp"
@del ".\\fonline_q_client.lib"
@del ".\\vc90.idb"

@pause