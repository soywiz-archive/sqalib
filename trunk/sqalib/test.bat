@echo off
del /Q _test.exe 2> NUL
cl /nologo sqal_stream.cpp /link /OUT:_test.exe && _test.exe
del /Q sqal_stream.obj 2> NUL
del /Q _test.exe 2> NUL