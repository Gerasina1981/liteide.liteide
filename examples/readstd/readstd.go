package main

import (
	"fmt"
	"os"
)

func main() {
	fmt.Println("Hello World, Type any text echo or quit to exit!")
	var s = [256]byte{}
	for {
		n, e := os.Stdin.Read(s[:])
		if e != nil || string(s[0:4]) == "quit" {
			break
		} 
		fmt.Println(s[0:n])
	}
}
