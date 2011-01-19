package main

import (
	"fmt"
	"./user32"
)

func main() {
	fmt.Println("Hello World! Hello 中国!")
	user32.MessageBoxW(0, "Hello World! Hello 中国!", "Hello")
}
