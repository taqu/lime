SET SHADERDIR=.\src\render\shader\
SET COMPILER=..\..\..\ShaderCompiler.exe

pushd %SHADERDIR%
%COMPILER% -vs -p5 Layout_P.vs Layout_P.byte
%COMPILER% -vs -p5 Layout_PN.vs Layout_PN.byte
%COMPILER% -vs -p5 Layout_PU.vs Layout_PU.byte
%COMPILER% -vs -p5 Layout_PC.vs Layout_PC.byte
%COMPILER% -vs -p5 Layout_PNTB.vs Layout_PNTB.byte
%COMPILER% -vs -p5 Layout_PNU.vs Layout_PNU.byte
%COMPILER% -vs -p5 Layout_PNCU.vs Layout_PNCU.byte
%COMPILER% -vs -p5 Layout_PNTBU.vs Layout_PNTBU.byte
%COMPILER% -vs -p5 Layout_PNUBone.vs Layout_PNUBone.byte
%COMPILER% -vs -p5 Layout_PNTUBone.vs Layout_PNTUBone.byte
%COMPILER% -vs -p5 Layout_Particle.vs Layout_Particle.byte
%COMPILER% -vs -p5 Layout_Sprite.vs Layout_Sprite.byte
%COMPILER% -vs -p5 Layout_Volume.vs Layout_Volume.byte
popd