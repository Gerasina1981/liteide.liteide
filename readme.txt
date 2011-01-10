LiteIDE

golang developer editor

*) UTF8 file load and save
*) Syntax Highliter
*) Project Manager
*) Project Build and run
*) No project, file build and run
*) gopromake auto build tools

Developer:  visualfc (visualfc@gamil.com)
Project : 	   http://code.google.com/p/liteide

usage:
set path=%path%;c:\go\bin

FAQ:

1. Project Manager
   LiteIDE open project, .edit pro file and save, auto reload project.

2. Project Editor

TARGET=cat
DESTDIR=./bin
GOFILES = \
	file.go \
	cat.go


3. Open project or file
  file open project/open or drop file (.pro/.go) to liteide ui

4.Build and Run
  Is Load Project, build and run project.
  Is Not load Project, build and run current editor file.