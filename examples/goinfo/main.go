package main

import (
	"fmt"
	"./goinfo"
)

func main() {
	fmt.Println("Test goinfo use local package")
	goinfo.PrintInfo()
}