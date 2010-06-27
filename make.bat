
@IF "%OPENCL_ROOT%"=="" @GOTO USAGE
@IF "%LOCAL_ROOT%"=="" @SET LOCAL_ROOT=%CD%
@SET DIRS=math environment compiler query imgfilter yuv pid nbody

@IF "%1"=="" SET TARGET=all
@IF NOT "%1"=="" SET TARGET=%1
IF "%TARGET%" == "symbols" @GOTO SYMBOLS

@FOR %%A IN (%DIRS%) DO @(
	@PUSHD sources\%%A
	make %TARGET% DEBUG=1
	@POPD
)
@GOTO END

:SYMBOLS
dumpbin /EXPORTS %WINDIR%\system32\nvcuda.dll > out\nvcuda.txt
dumpbin /EXPORTS %WINDIR%\system32\OpenCL.dll > out\OpenCL.txt
dumpbin /IMPORTS out\clCompiler.exe > out\clCompiler.txt
@GOTO END

:USAGE
ECHO You must set OPENCL_ROOT (which includes the headers and the .lib).

:END