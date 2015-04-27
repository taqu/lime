@echo off
call "%VS110COMNTOOLS%vsvars32.bat"

devenv lime_dx11_vc120.sln /Clean ReleaseSSE

devenv lime_dx11_vc120.sln /Build ReleaseSSE
