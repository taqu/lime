SET SHADERDIR=.\shader_source\
SET OUTDIR=..\shader\
SET COMPILER=..\tools\ShaderCompiler.exe

pushd %SHADERDIR%
%COMPILER% -vs -p5 -compress -text LightDepth.vs %OUTDIR%LightDepthVS.txt
%COMPILER% -vs -p5 -compress -text LightDepthSkinning.vs %OUTDIR%LightDepthSkinningVS.txt
%COMPILER% -gs -p5 -compress -text LightDepth.gs %OUTDIR%LightDepthGS.txt

%COMPILER% -vs -p5 -compress -text Deferred.vs %OUTDIR%DeferredVS.txt
%COMPILER% -vs -p5 -compress -text DeferredSkinning.vs %OUTDIR%DeferredSkinningVS.txt
%COMPILER% -vs -p5 -compress -text DeferredUV.vs %OUTDIR%DeferredUVVS.txt
%COMPILER% -vs -p5 -compress -text DeferredUVSkinning.vs %OUTDIR%DeferredUVSkinningVS.txt
%COMPILER% -vs -p5 -compress -text DeferredNUV.vs %OUTDIR%DeferredNUVVS.txt
%COMPILER% -vs -p5 -compress -text DeferredNUVSkinning.vs %OUTDIR%DeferredNUVSkinningVS.txt

%COMPILER% -ps -p5 -compress -text Deferred.ps %OUTDIR%DeferredPS.txt
%COMPILER% -ps -p5 -compress -text DeferredUV.ps %OUTDIR%DeferredUVPS.txt
%COMPILER% -ps -p5 -compress -text DeferredTexC.ps %OUTDIR%DeferredTexCPS.txt
%COMPILER% -ps -p5 -compress -text DeferredTexCN.ps %OUTDIR%DeferredTexCNPS.txt

%COMPILER% -vs -p5 -compress -text Forward.vs %OUTDIR%ForwardVS.txt
%COMPILER% -vs -p5 -compress -text ForwardShadow.vs %OUTDIR%ForwardShadowVS.txt
%COMPILER% -vs -p5 -compress -text ForwardSkinning.vs %OUTDIR%ForwardSkinningVS.txt
%COMPILER% -vs -p5 -compress -text ForwardSkinningShadow.vs %OUTDIR%ForwardSkinningShadowVS.txt
%COMPILER% -vs -p5 -compress -text ForwardUV.vs %OUTDIR%ForwardUVVS.txt
%COMPILER% -vs -p5 -compress -text ForwardUVShadow.vs %OUTDIR%ForwardUVShadowVS.txt
%COMPILER% -vs -p5 -compress -text ForwardUVSkinning.vs %OUTDIR%ForwardUVSkinningVS.txt
%COMPILER% -vs -p5 -compress -text ForwardUVSkinningShadow.vs %OUTDIR%ForwardUVSkinningShadowVS.txt
%COMPILER% -vs -p5 -compress -text ForwardNUV.vs %OUTDIR%ForwardNUVVS.txt
%COMPILER% -vs -p5 -compress -text ForwardNUVShadow.vs %OUTDIR%ForwardNUVShadowVS.txt
%COMPILER% -vs -p5 -compress -text ForwardNUVSkinning.vs %OUTDIR%ForwardNUVSkinningVS.txt
%COMPILER% -vs -p5 -compress -text ForwardNUVSkinningShadow.vs %OUTDIR%ForwardNUVSkinningShadowVS.txt

%COMPILER% -ps -p5 -compress -text Forward.ps %OUTDIR%ForwardPS.txt
%COMPILER% -ps -p5 -compress -text ForwardShadow.ps %OUTDIR%ForwardShadowPS.txt
%COMPILER% -ps -p5 -compress -text ForwardUV.ps %OUTDIR%ForwardUVPS.txt
%COMPILER% -ps -p5 -compress -text ForwardUVShadow.ps %OUTDIR%ForwardUVShadowPS.txt
%COMPILER% -ps -p5 -compress -text ForwardTexC.ps %OUTDIR%ForwardTexCPS.txt
%COMPILER% -ps -p5 -compress -text ForwardTexCShadow.ps %OUTDIR%ForwardTexCShadowPS.txt
%COMPILER% -ps -p5 -compress -text ForwardTexCN.ps %OUTDIR%ForwardTexCNPS.txt
%COMPILER% -ps -p5 -compress -text ForwardTexCNShadow.ps %OUTDIR%ForwardTexCNShadowPS.txt

popd
