LiteIDE

Simple go code edit and build IDE

* Project or Makefile manage
* Project/Makefile/Gofile build and clean
* Project multi packages build
* Tools gopromake - project or file build
* Tools goastview - go source file ast view
* UTF8 file load and save
* Syntax highlighter
* Build error jump to source line 

OS: 
     Windows
     Linux x86_64 
     MacOSX 10.6

Developer:  visualfc (visualfc@gmail.com)
Project :   http://code.google.com/p/golang


FAQ:

1. Project Manager
   LiteIDE open project, .edit pro file and save, auto reload project.

2. Project Editor

TARGET=hello
DESTDIR=./bin
GCOPT=
GLOPT=
GOFILES = \
	hello.go \
	user32.go


3. Open project or file
  file open project/open or drop file (.pro/.go) to liteide ui

4.Build and Run
  Is Load Project, build and run project.
  Is Not load Project, build and run current editor file.
