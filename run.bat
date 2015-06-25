:run.bat

@echo off


echo.

start CppCli-WPF-App/Debug/Server.exe 9996 "./roots/server1"
start CppCli-WPF-App/Debug/Server.exe 9997 "./roots/server2"
"CppCli-WPF-App/Debug/CppCli-WPF-App.exe" 2345
echo.


PAUSE

