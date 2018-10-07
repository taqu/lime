SET SHADERDIR=.\shader\
SET OUTDIR=.\
SET COMPILER=..\..\tools\ShaderCompiler.exe

pushd %SHADERDIR%
%COMPILER% -vs -p5 -compress -text ProceduralSpace.vs %OUTDIR%ProceduralSpaceVS.txt

%COMPILER% -ps -p5 -compress -text ProceduralSpace.ps %OUTDIR%ProceduralSpacePS.txt
%COMPILER% -ps -p5 -compress -text ProceduralTerrain00.ps %OUTDIR%ProceduralTerrain00PS.txt

popd
