package main

import (
	"flag"
	"fmt"
	"os"
)

var (
	flagInputSrc = flag.String("src", "", "input go source file")
	flagStdin    = flag.Bool("stdin", false, "input by stdin")
	flagInputPro = flag.String("pro", "", "input go project file")
)

func main() {
	flag.Parse()
	if len(*flagInputSrc) == 0 {
		flag.Usage()
		os.Exit(1)
	}
	if len(*flagInputSrc) > 0 {
		var f *os.File
		if *flagStdin {
			f = os.Stdin
		} else {
			f = os.NewFile(os.O_RDONLY, *flagInputSrc)
		}
		view, err := NewFilePackageSource(*flagInputSrc, f)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error:%s", err)
			os.Exit(1)
		}
		view.PrintTree(os.Stdout)
	}
	os.Exit(0)
}
