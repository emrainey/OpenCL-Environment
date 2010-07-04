@SETLOCAL
@IF "%OPENCL_ROOT%"=="" @GOTO USAGE
@IF "%LOCAL_ROOT%"=="" @SET LOCAL_ROOT=%CD%
@SET DIRS=math environment query devicequery compiler imgfilter yuv pid nbody

@IF "%1"=="" SET TARGET=all
@IF NOT "%1"=="" SET TARGET=%1
@IF "%TARGET%" == "symbols" @GOTO SYMBOLS
@IF "%TARGET%" == "scrub" @GOTO SCRUB
@GOTO TARGETS

:SCRUB
SET TYPES=lib pdb idb exp obj exe o a so ilk manifest
FOR %%A IN (%TYPES%) DO del /Q /F out\obj\*.%%A
FOR %%A IN (%TYPES%) DO del /Q /F out\*.%%A
@GOTO END

:TARGETS
@FOR %%A IN (%DIRS%) DO @(
	@PUSHD sources\%%A
	@IF "%CL_DEBUG%"=="1" make %TARGET% DEBUG=1
	@IF "%CL_DEBUG%"=="" make %TARGET%
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
@ENDLOCAL
