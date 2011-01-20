package main

import (
	"flag"
	"fmt"
	"os"
)

var (
	flagInputSrc = flag.String("src","","input go source file")	
	flagInputPro = flag.String("pro","","input go project file")
)	

func main() {
	flag.Parse()
	if len(*flagInputSrc) == 0 {
		flag.Usage()
		os.Exit(1)
	}
	if len(*flagInputSrc) > 0 {
		view,err := NewFilePackage(*flagInputSrc)
		if err != nil {
			fmt.Printf("Error:%s",err)
			os.Exit(1)
		}
		view.PrintTree(os.Stdout)
	}
}
