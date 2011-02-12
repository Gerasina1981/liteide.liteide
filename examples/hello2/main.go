package main

import "fmt"
import "syscall"

func main() {
	print_hello()
	for i := 0; i < 3000; i++ {
		fmt.Printf("%d\n",i)
		syscall.Sleep(10)
	}
}
