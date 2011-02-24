package main

import (
	"fmt"
)

func main() {	
	fmt.Println("Hello World!")
	m1 := []byte("hello")
	m2 := []byte("hello")
	m3 := append(m1,string(m2))
	fmt.Println(m1)
}
