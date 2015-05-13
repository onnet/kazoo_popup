QuaZip - How to build
=========================

* Windows
            
			1) add into quazip.pro file:
				win32 {
				INCLUDEPATH += $$[QT_INSTALL_PREFIX]/../Src/qtbase/src/3rdparty/zlib
				}
			
            2) Set Qt environment: run $$QT_DIR\bin\qtenv2.bat 
                e.g. c:\Qt\Qt5.3.1\5.3\mingw482_32\bin\qtenv2.bat

			3) qmake quazip.pro

			4) mingw32-make -jN* release
				
* Mac OS
			
			1) add into quazip.pro file:
				macx {
				LIBS += -L/usr/local/lib -lz
				}
	
            2) Set Qt environment: export PATH=$QT_DIR/bin:$PATH
                e.g. export PATH=/Users/Alex/Qt5.3.0/5.3/clang_64/bin:$PATH

			3) qmake quazip.pro
			
			4) make -jN*

\* N in -jN is the number of CPU cores on your system	