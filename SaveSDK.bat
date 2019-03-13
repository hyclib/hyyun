#demo

xcopy "include\*.*" 	"..\..\..\hy_bin\latest\include\hyyun\" /S /Y
xcopy "document\*.*" 	"..\..\..\hy_bin\latest\hyyun\" /S /Y
xcopy "test\*.h" 		"..\..\..\hy_bin\latest\hyyun\demo\" /S /Y
xcopy "test\*.hpp" 		"..\..\..\hy_bin\latest\hyyun\demo\" /S /Y
xcopy "test\*.cpp" 		"..\..\..\hy_bin\latest\hyyun\demo\" /S /Y

#vc9

xcopy "bin\Debug\hyyun.dll"  		"..\..\..\hy_bin\latest\libs\win32\Debug\" /S /Y
xcopy "bin\Debug\hyyun.lib"  		"..\..\..\hy_bin\latest\libs\win32\Debug\" /S /Y
xcopy "bin\Debug\hyyun.pdb"  		"..\..\..\hy_bin\latest\libs\win32\Debug\" /S /Y
xcopy "bin\Debug.unicode\hyyun.dll"  	"..\..\..\hy_bin\latest\libs\win32\Debug.unicode\" /S /Y
xcopy "bin\Debug.unicode\hyyun.lib"  	"..\..\..\hy_bin\latest\libs\win32\Debug.unicode\" /S /Y
xcopy "bin\Debug.unicode\hyyun.pdb"  	"..\..\..\hy_bin\latest\libs\win32\Debug.unicode\" /S /Y
xcopy "bin\Release\hyyun.dll"  		"..\..\..\hy_bin\latest\libs\win32\Release\" /S /Y
xcopy "bin\Release\hyyun.lib"  		"..\..\..\hy_bin\latest\libs\win32\Release\" /S /Y
xcopy "bin\Release.unicode\hyyun.dll"  	"..\..\..\hy_bin\latest\libs\win32\Release.unicode\" /S /Y
xcopy "bin\Release.unicode\hyyun.lib"  	"..\..\..\hy_bin\latest\libs\win32\Release.unicode\" /S /Y


#vc12

xcopy "bin.vc12\Debug\hyyun.dll"  		"..\..\..\hy_bin\latest\libs\win32.vc12\Debug\" /S /Y
xcopy "bin.vc12\Debug\hyyun.lib"  		"..\..\..\hy_bin\latest\libs\win32.vc12\Debug\" /S /Y
xcopy "bin.vc12\Debug\hyyun.pdb"  		"..\..\..\hy_bin\latest\libs\win32.vc12\Debug\" /S /Y
xcopy "bin.vc12\Debug.unicode\hyyun.dll"  	"..\..\..\hy_bin\latest\libs\win32.vc12\Debug.unicode\" /S /Y
xcopy "bin.vc12\Debug.unicode\hyyun.lib"  	"..\..\..\hy_bin\latest\libs\win32.vc12\Debug.unicode\" /S /Y
xcopy "bin.vc12\Debug.unicode\hyyun.pdb"  	"..\..\..\hy_bin\latest\libs\win32.vc12\Debug.unicode\" /S /Y
xcopy "bin.vc12\Release\hyyun.dll"  		"..\..\..\hy_bin\latest\libs\win32.vc12\Release\" /S /Y
xcopy "bin.vc12\Release\hyyun.lib"  		"..\..\..\hy_bin\latest\libs\win32.vc12\Release\" /S /Y
xcopy "bin.vc12\Release.unicode\hyyun.dll"  	"..\..\..\hy_bin\latest\libs\win32.vc12\Release.unicode\" /S /Y
xcopy "bin.vc12\Release.unicode\hyyun.lib"  	"..\..\..\hy_bin\latest\libs\win32.vc12\Release.unicode\" /S /Y


