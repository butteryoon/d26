package main 

import (
	"fmt"
)

func generate(ch chan int) {
	for i := 2; ; i++ {
		fmt.Println("generate: ", i)
		ch <- i
	}
}

func filter(in, out chan int, prime int) {
	fmt.Println("filter create: ", prime)
	for {
		i := <- in
		fmt.Println("filter.", prime, "in: ", i)
		if i % prime != 0 {
			out <- i
			fmt.Println("filter.", prime, "out: ", i)
		}
	}
}

func main() {
	ch := make(chan int)
	
	go generate(ch)
	
	for i := 0; i < 5; i++ {
		prime := <- ch
		
		fmt.Println("prime: ", prime)
		ch1 := make(chan int)
		fmt.Println("filter create befoe ", prime)
		go filter(ch, ch1, prime)
		
		ch = ch1
	}
}

