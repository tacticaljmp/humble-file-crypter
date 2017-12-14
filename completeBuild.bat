@ECHO OFF
SET ROOTDRIVE=%SystemDrive%
SET MINGWDIR=MINGW
SET MINGWBIN=BIN
SET GCCCOMPILER=GCC.EXE
SET GPPCOMPILER=G++.EXE

SET GCC=%ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\%GCCCOMPILER%
SET GPP=%ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\%GPPCOMPILER%

ECHO GCC COMPILER = %GCC%
ECHO G++ COMPILER = %GPP%

ECHO Copy Libs To Local Path (%CD%)

COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libgmp-10.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libgcc_s_dw2-1.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libmpc-3.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libmpfr-4.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\zlib1.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libintl-8.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libiconv-2.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libmingwex-0.dll %CD%
COPY %ROOTDRIVE%\%MINGWDIR%\%MINGWBIN%\libisl-15.dll %CD%

echo Compiling payloadConverter.c into payloadConverter.exe ...
%GCC% payloadConverter.c -o payloadConverter.exe -lmsvcrt

echo Constructing payloads.h from payloads.txt and dropfiles.txt ...
payloadConverter.exe payloads.txt dropfiles.txt > payloads.h

echo Building crypted executable package ...
%GPP% crypter.c -o crypter.exe -static -s -O5

echo Cleaning Up Temp Files...

DEL %CD%\libgmp-10.dll
DEL %CD%\libgcc_s_dw2-1.dll
DEL %CD%\libmpc-3.dll
DEL %CD%\libmpfr-4.dll
DEL %CD%\zlib1.dll
DEL %CD%\libintl-8.dll
DEL %CD%\libiconv-2.dll
DEL %CD%\libmingwex-0.dll
DEL %CD%\libisl-15.dll
