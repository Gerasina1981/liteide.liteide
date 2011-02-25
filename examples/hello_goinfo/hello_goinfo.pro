# hello_goinfo project file

TARGET = hello_goinfo
DESTDIR =

GCOPT = -I ../goinfo/_obj/goinfo
GLOPT = -L ../goinfo/_obj/goinfo

GOFILES = \
	main.go \
	doc.go