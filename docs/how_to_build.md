KazooPopup - How to build
=========================

## Dependencies

| Name         | Version                               |
|--------------|---------------------------------------|
| Qt           | >= 5.3.0 (using websockets module)    |
| C++ compiler | supporting C++11 (i.e. gcc 4.6+)      |
| NSIS		   | >= 2.46 (for setup on Windows)		   |

## Using QtCreator

Just open `KazooPopup.pro` file and press 'Build'.

## Using terminal

* Windows
            
            1) Set Qt environment: run $$QT_DIR\bin\qtenv2.bat 
                e.g. c:\Qt\Qt5.3.1\5.3\mingw482_32\bin\qtenv2.bat

			2) qmake KazooPopup.pro

			3) mingw32-make -jN* release
				
* Mac OS
	
            1) Set Qt environment: export PATH=$QT_DIR/bin:$PATH
                e.g. export PATH=/Users/Alex/Qt5.3.0/5.3/clang_64/bin:$PATH

			2) qmake KazooPopup.pro
			
			3) make -jN*

## Using Jenkins

* Windows

            cd KazooPopup
            pushd .
            ; Set Qt environment
            call $$QT_DIR\bin\qtenv2.bat ;(e.g. c:\Qt\Qt5.3.1\5.3\mingw482_32\bin\qtenv2.bat)
            
            popd
            
            rd /s /q build
            mkdir build
            
            pushd build
            
            qmake ..\KazooPopup.pro
            
            mingw32-make -jN* release ;(e.g. mingw32-make -j4 release)
            
            copy /y release\KazooPopup.exe setup

            del setup\*.exe

            "c:\Program Files (x86)\NSIS\makensis.exe" setup\setup.nsi
            
            popd
            
* Mac OS

            cd KazooPopup
            pushd .
            # Set Qt environment
            export PATH=$QT_DIR/bin:$PATH #(e.g. export PATH=/Users/Alex/Qt5.3.0/5.3/clang_64/bin:$PATH)
            
            popd
            
            rm -rf build
            mkdir build
            
            pushd build
            
            qmake ../KazooPopup.pro
            
            make -jN* #(e.g. make -j4)
            
            make deploy
            
            popd
			
\* N in -jN is the number of CPU cores on your system			