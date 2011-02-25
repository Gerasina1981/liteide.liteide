// goinfo project goinfo.go
package goinfo

import (
	"runtime"
	"fmt"
)

func PrintInfo() {
	fmt.Println("GOROOT = ",runtime.GOROOT())
	fmt.Println("GOARCH = ",runtime.GOARCH)	
	fmt.Println("GOOS = ",runtime.GOOS)
	fmt.Println("VERSION = ",runtime.Version())
}