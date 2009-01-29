set INCLUDES=%INCLUDES% -I"C:\Program Files\Microsoft Visual C++ Toolkit 2003\include" -I"C:\Program Files\Microsoft Platform SDK\Include"
set LIBS=/LIBPATH:"C:\Program Files\Microsoft Platform SDK\Lib" /LIBPATH:"C:\Program Files\Microsoft Visual C++ Toolkit 2003\lib" 
set PATH=%PATH%;"C:\Program Files\Microsoft Visual C++ Toolkit 2003\bin"
nmake -f makefile.vc7 %1 %2 %3
