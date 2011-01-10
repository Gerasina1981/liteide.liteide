package main

import(
	"flag"
	"fmt"
)

func main() {
	flag.Parse()
	narg := flag.NArg()
	if narg < 1 {
		fmt.Printf("Go source\n")
		return
	}
	
	fmt.Println(flag.Args())	
	array := ParserFiles(flag.Args())
	fmt.Println(array)
}
