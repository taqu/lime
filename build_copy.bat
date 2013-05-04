set SRCDIR=%~dp0
set OUTDIR=%SRCDIR%build\

if not exist %OUTDIR% mkdir %OUTDIR%
REM echo %SRCDIR%
REM echo %OUTDIR%
REM echo %DOCDIR%
for %%i in (lcore lmath lgraphics lframework linput lsound ext xml) do xcopy %SRCDIR%%%i\*.h %OUTDIR%%%i\ /S /Y /R

xcopy %SRCDIR%doc\* %OUTDIR%doc\ /S /Y /R
xcopy %SRCDIR%lib\*.lib %OUTDIR%lib\ /S /Y /R
xcopy %SRCDIR%lib\*.pdb %OUTDIR%lib\ /S /Y /R
