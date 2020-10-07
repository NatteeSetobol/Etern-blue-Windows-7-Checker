@echo off
REM remember to set the path to the include and lib directory
set Libs=User32.lib comdlg32.lib gdi32.lib libeay32.lib ssleay32.lib -I../lib/include /link /libpath:../lib/lib
set CPPFiles=required/memory.cpp stringz.cpp ssl.cpp bucket.cpp httpprotocol.cpp marray.cpp win_threaded_queue.cpp token.cpp win_socket.cpp socket.cpp required/nix.cpp View_Memory.cpp WindowsObjects.cpp 
set flags=-DUSE_SSL -DCOMPILER_MSVC -wd4067
cl main.cpp %CPPFiles% %Flags% %Libs%
