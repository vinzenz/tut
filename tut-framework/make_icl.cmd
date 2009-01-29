set PATH=%PATH%;"C:\Program Files\Intel\Compiler60\IA32\Bin";;"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin";"C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE"
set INCLUDES=%INCLUDES% -I"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\include" -I"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Include"
set LIBS=/LIBPATH:"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\lib" /LIBPATH:"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Lib" /LIBPATH:"C:\Program Files\Intel\Compiler60\IA32\Lib"
make -f makefile.icl %1 %2
