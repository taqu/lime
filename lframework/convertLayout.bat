SET SHADERDIR=.\shader_source\layout\
SET OUTDIR=.\shader\
SET COMPILER=.\tools\ShaderCompiler.exe

%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_P.vs %OUTDIR%Layout_P.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PN.vs %OUTDIR%Layout_PN.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PU.vs %OUTDIR%Layout_PU.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PC.vs %OUTDIR%Layout_PC.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PNTB.vs %OUTDIR%Layout_PNTB.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PNU.vs %OUTDIR%Layout_PNU.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PNCU.vs %OUTDIR%Layout_PNCU.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PNTBU.vs %OUTDIR%Layout_PNTBU.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PNUBone.vs %OUTDIR%Layout_PNUBone.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_PNTUBone.vs %OUTDIR%Layout_PNTUBone.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_Sprite2D.vs %OUTDIR%Layout_Sprite2D.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_Particle.vs %OUTDIR%Layout_Particle.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_Sprite.vs %OUTDIR%Layout_Sprite.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_VolumeParticle.vs %OUTDIR%Layout_VolumeParticle.txt
%COMPILER% -vs -p5 -compress -text %SHADERDIR%Layout_UI.vs %OUTDIR%Layout_UI.txt
