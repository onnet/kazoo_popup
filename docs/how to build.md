KazooPopup - How to build
=========================

## Dependencies

| Name         | Version                               |
|--------------|---------------------------------------|
| Qt           | >= 5.3.0 (using websockets module)    |
| C++ compiler | supporting C++11 (i.e. gcc 4.6+)      |

## Instructions

* Windows
            
            1) Set Qt environment: run $$QT_DIR\bin\qtenv2.bat 
                e.g. c:\Qt\Qt5.3.1\5.3\mingw482_32\bin\qtenv2.bat

			2) qmake KazooPopup.pro

			3) mingw32-make -jN* release
				* N in -jN is the number of CPU cores on your system
* Mac OS
	
            1) Set Qt environment: export PATH=$QT_DIR/bin:$PATH
                e.g. export PATH=/Users/Alex/Qt5.3.0/5.3/clang_64/bin:$PATH

			2) qmake KazooPopup.pro
			
			3) make -jN*
				* N in -jN is the number of CPU cores on your system
            
            4) make deploy
            