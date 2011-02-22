package main

import "fmt"
import "syscall"

func main() {
	print_hello()
	for i := 0; i < 30000; i++ {
		fmt.Printf("%d\n",i)
		syscall.Sleep(10)
	}
	test()
	test2()
}

func test() {
	for i := 0; i < 3000; i++ {
		fmt.Printf("%d\n",i)
		syscall.Sleep(100)
	}
}

func test2() {
	for i:= 0; i < 3000; i++ {
		fmt.Printf("%s",i)
		syscall.Sleep(200)
	}
}

