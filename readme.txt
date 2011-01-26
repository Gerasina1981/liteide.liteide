LiteIDE

Simple go code edit and build IDE

* UTF8 file load and save
* Syntax highliter
* Go source file build and run, if no project
* Project manager, multi packages support
* Project auto build tool gopromake
* Go souce file ast view tool goastview
* Build error jump to source line 

Developer:  visualfc (visualfc@gmail.com)
Project :   http://code.google.com/p/liteide


FAQ:

1. Project Manager
   LiteIDE open project, .edit pro file and save, auto reload project.

2. Project Editor

TARGET=hello
DESTDIR=./bin
GOFILES = \
	hello.go \
	user32.go


3. Open project or file
  file open project/open or drop file (.pro/.go) to liteide ui

4.Build and Run
  Is Load Project, build and run project.
  Is Not load Project, build and run current editor file.
