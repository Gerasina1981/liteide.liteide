// goinfo project doc.go

/*
goinfo document

main.pro : use local package
1.main.go
import "./goinfo"
2.main.pro
GOFILE += goinfo.go

main2.pro : use search package
1. first build goinfo.pro
2. main2.go 
import "goinfo"
3. main2.pro
GCOPT += -I _obj/goinfo

goinfo.pro : goinfo package
DESTDIR = _obj/goinfo 

*/
package documentation
