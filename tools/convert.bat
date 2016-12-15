SET SHADERDIR=.\src\render\shader\
SET COMPILER=..\..\..\ShaderCompiler.exe

pushd %SHADERDIR%

%COMPILER% -vs -p5 Depth.vs DepthVS.byte
%COMPILER% -vs -p5 DepthSkinning.vs DepthSkinningVS.byte
%COMPILER% -gs -p5 Depth.gs DepthGS.byte

%COMPILER% -vs -p5 Normal.vs NormalVS.byte
%COMPILER% -vs -p5 NormalNoUV.vs NormalNoUVVS.byte
%COMPILER% -vs -p5 NormalSkinning.vs NormalSkinningVS.byte
%COMPILER% -vs -p5 NormalShadow.vs NormalShadowVS.byte
%COMPILER% -vs -p5 NormalNoUVShadow.vs NormalNoUVShadowVS.byte
%COMPILER% -vs -p5 NormalSkinningShadow.vs NormalSkinningShadowVS.byte
%COMPILER% -vs -p5 NormalMap.vs NormalMapVS.byte
%COMPILER% -vs -p5 NormalMapSkinning.vs NormalMapSkinningVS.byte
%COMPILER% -vs -p5 NormalMapShadow.vs NormalMapShadowVS.byte
%COMPILER% -vs -p5 NormalMapSkinningShadow.vs NormalMapSkinningShadowVS.byte

%COMPILER% -vs -p5 NormalMotion.vs NormalMotionVS.byte
%COMPILER% -vs -p5 NormalNoUVMotion.vs NormalNoUVMotionVS.byte
%COMPILER% -vs -p5 NormalSkinningMotion.vs NormalSkinningMotionVS.byte

%COMPILER% -ps -p5 Normal.ps NormalPS.byte
%COMPILER% -ps -p5 NormalNoUV.ps NormalNoUVPS.byte
%COMPILER% -ps -p5 NormalShadow.ps NormalShadowPS.byte
%COMPILER% -ps -p5 NormalNoUVShadow.ps NormalNoUVShadowPS.byte
%COMPILER% -ps -p5 NormalTexC.ps NormalTexCPS.byte
%COMPILER% -ps -p5 NormalTexCShadow.ps NormalTexCShadowPS.byte
%COMPILER% -ps -p5 NormalTexCN.ps NormalTexCNPS.byte
%COMPILER% -ps -p5 NormalTexCNShadow.ps NormalTexCNShadowPS.byte

%COMPILER% -ps -p5 NormalMotion.ps NormalMotionPS.byte

%COMPILER% -vs -p5 Rectangle.vs RectangleVS.byte
%COMPILER% -ps -p5 Rectangle.ps RectanglePS.byte

%COMPILER% -vs -p5 RectangleColor.vs RectangleColorVS.byte
%COMPILER% -ps -p5 RectangleColor.ps RectangleColorPS.byte

%COMPILER% -vs -p5 Terrain.vs TerrainVS.byte
%COMPILER% -ps -p5 Terrain.ps TerrainPS.byte

%COMPILER% -vs -p5 Particle.vs ParticleVS.byte
%COMPILER% -gs -p5 Particle.gs ParticleGS.byte
%COMPILER% -ps -p5 Particle.ps ParticlePS.byte
%COMPILER% -ps -p5 ParticleUber.ps ParticleUberPS.byte

%COMPILER% -vs -p5 DebugVC.vs DebugVCVS.byte
%COMPILER% -ps -p5 DebugVC.ps DebugVCPS.byte

%COMPILER% -vs -p5 Sprite.vs SpriteVS.byte
%COMPILER% -vs -p5 Sprite2D.vs Sprite2DVS.byte
%COMPILER% -ps -p5 Sprite.ps SpritePS.byte

REM %COMPILER% -ps -p5 TonemappingStatic.ps TonemappingStaticPS.byte

%COMPILER% -vs -p5 FullQuad.vs FullQuadVS.byte

%COMPILER% -cs -p5 Bloom.scs BloomCS.byte
%COMPILER% -cs -p5 GaussH.scs GaussHCS.byte
%COMPILER% -cs -p5 GaussV.scs GaussVCS.byte
%COMPILER% -cs -p5 Downsample.scs DownsampleCS.byte
%COMPILER% -cs -p5 Upsample.scs UpsampleCS.byte

REM %COMPILER% -vs -p5 FXAA.vs FXAAVS.byte
REM %COMPILER% -ps -p5 FXAA.ps FXAAPS.byte
%COMPILER% -cs -p5 FXAA.scs FXAACS.byte

%COMPILER% -ps -p5 HDAO.ps HDAOPS.byte

%COMPILER% -cs -p5 ReconstructZ.scs ReconstructZCS.byte
%COMPILER% -cs -p5 Downsample2x2.scs Downsample2x2CS.byte
%COMPILER% -cs -p5 Upsample.scs UpsampleCS.byte
%COMPILER% -ps -p5 SAO.ps SAOPS.byte
%COMPILER% -ps -p5 SAOBlur.ps SAOBlurPS.byte

%COMPILER% -cs -p5 CameraMotion.scs CameraMotionCS.byte
%COMPILER% -cs -p5 FilterTileMax.scs FilterTileMaxCS.byte
%COMPILER% -cs -p5 FilterNeighborMax.scs FilterNeighborMaxCS.byte
%COMPILER% -cs -p5 AngularVariance.scs AngularVarianceCS.byte
%COMPILER% -cs -p5 MotionBlur.scs MotionBlurCS.byte
%COMPILER% -cs -p5 Reconstruct.scs ReconstructCS.byte
%COMPILER% -cs -p5 TAA.scs TAACS.byte

%COMPILER% -cs -p5 ReconstructZ.scs ReconstructZCS.byte
%COMPILER% -cs -p5 ReconstructLinearZNormal.scs ReconstructLinearZNormalCS.byte
%COMPILER% -cs -p5 AlchemyAO.scs AlchemyAOCS.byte
%COMPILER% -cs -p5 AlchemyAOBlur.scs AlchemyAOBlurCS.byte

%COMPILER% -vs -p5 SkyBox.vs SkyBoxVS.byte
%COMPILER% -ps -p5 SkyBox.ps SkyBoxPS.byte

REM %COMPILER% -cs -p5 CoCBlurH.scs CoCBlurHCS.byte
REM %COMPILER% -cs -p5 CoCBlurV.scs CoCBlurVCS.byte
REM %COMPILER% -cs -p5 Bokeh.scs BokehCS.byte
REM %COMPILER% -cs -p5 DoFH.scs DoFHCS.byte
REM %COMPILER% -cs -p5 DoFV.scs DoFVCS.byte
%COMPILER% -cs -p5 DoFFilter.scs DoFFilterCS.byte
%COMPILER% -ps -p5 DoFFilterFinal.ps DoFFilterFinalPS.byte

%COMPILER% -vs -p5 Bezier.vs BezierVS.byte
%COMPILER% -hs -p5 Bezier.hs BezierHS.byte
%COMPILER% -ds -p5 Bezier.ds BezierDS.byte
%COMPILER% -ps -p5 Bezier.ps BezierPS.byte

%COMPILER% -cs -p5 VSMReconstruct.scs VSMReconstructCS.byte
%COMPILER% -cs -p5 VSMBlur.scs VSMBlurCS.byte

REM %COMPILER% -vs -p5 Volume.vs VolumeVS.byte
REM %COMPILER% -hs -p5 Volume.hs VolumeHS.byte
REM %COMPILER% -ds -p5 Volume.ds VolumeDS.byte
REM %COMPILER% -ps -p5 Volume.ps VolumePS.byte

%COMPILER% -vs -p5 Explosion.vs ExplosionVS.byte
%COMPILER% -hs -p5 Explosion.hs ExplosionHS.byte
%COMPILER% -ds -p5 Explosion.ds ExplosionDS.byte
%COMPILER% -ps -p5 Explosion.ps ExplosionPS.byte
popd
