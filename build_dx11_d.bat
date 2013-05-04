@echo off
call "%VS110COMNTOOLS%vsvars32.bat"

devenv lime_dx11_vc110.sln /Clean DebugSSE

devenv lime_dx11_vc110.sln /Build DebugSSE
