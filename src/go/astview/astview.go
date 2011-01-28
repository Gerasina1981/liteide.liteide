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
			var err os.Error
			f,err = os.Open( *flagInputSrc, os.O_RDONLY, 0)
			if err != nil {
				fmt.Fprintf(os.Stderr, "Error:%s", err)
				os.Exit(1)
			}
		}
		defer f.Close()
		view, err := NewFilePackageSource(*flagInputSrc, f)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error:%s", err)
			os.Exit(1)
		}
		view.PrintTree(os.Stdout)
	}
	os.Exit(0)
}
