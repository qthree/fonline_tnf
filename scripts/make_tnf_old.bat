@echo off
@set PATH=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 9.0\VC\BIN;C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools;C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools\bin;C:\Program Files\Microsoft Visual Studio 9.0\VC\PlatformSDK\bin;C:\Program Files\Microsoft Visual Studio 9.0\SDK\v2.0\bin;C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;C:\Program Files\Microsoft Visual Studio 9.0\VC\VCPackages;%PATH%
@set INCLUDE=C:\Program Files\Microsoft Visual Studio 9.0\VC\ATLMFC\INCLUDE;C:\Program Files\Microsoft Visual Studio 9.0\VC\INCLUDE;C:\Program Files\Microsoft Visual Studio 9.0\VC\PlatformSDK\include;C:\Program Files\Microsoft Visual Studio 9.0\SDK\v2.0\include;%INCLUDE%
@set LIB=C:\Program Files\Microsoft Visual Studio 9.0\VC\ATLMFC\LIB;C:\Program Files\Microsoft Visual Studio 9.0\VC\LIB;C:\Program Files\Microsoft Visual Studio 9.0\VC\PlatformSDK\lib;C:\Program Files\Microsoft Visual Studio 9.0\SDK\v2.0\lib;%LIB%;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib
@set LIBPATH=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;C:\Program Files\Microsoft Visual Studio 9.0\VC\ATLMFC\LIB

del ".\\fonline_tnf.dll"
cl.exe /nologo /MT /W3 /O2 /Gd /D "__SERVER" /Fo".\\" /Fd".\\fonline_tnf.obj" /FD /c ".\\fonline_tnf.cpp"
link.exe /nologo /dll /incremental:no /machine:I386 ".\\fonline_tnf.obj" /out:".\\fonline_tnf.dll"
del ".\\fonline_tnf.obj"
del ".\\fonline_tnf.exp"
del ".\\fonline_tnf.lib"
del ".\\fonline_tnf.idb"

del ".\\fonline_tnf_client.dll"
cl.exe /nologo /MT /W3 /O2 /Gd /D "__CLIENT" /Fo".\\fonline_tnf_client.obj" /Fd".\\" /FD /c ".\\fonline_tnf.cpp"
link.exe /nologo /dll /incremental:no /machine:I386 ".\\fonline_tnf_client.obj" /out:".\\fonline_tnf_client.dll"
del ".\\fonline_tnf_client.obj"
del ".\\fonline_tnf_client.exp"
del ".\\fonline_tnf_client.lib"
del ".\\vc90.idb"
copy ".\\fonline_tnf_client.dll" "..\\..\\cl_rp_sdk\\data\\scripts\\fonline_tnf_client.dll"



@pause