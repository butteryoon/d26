package main 

import (
	"fmt"
	"os"
	"io"
)

func main() {
	fmt.Println("Hello World !!!")
	
//	go server(1)
//	go server(2)
/*	
	fmt.Println(unhex('a'))

	if cnt := 3; cnt > 4 {
		fmt.Println("ok")
	}
*/
	
/*
	sum := 0
	for i := 0; i < 10; i++ {
		sum += i
	}
	fmt.Println(sum)
*/

/*
	var a [4]int

	a[0] = 2
	a[1] = 2
	a[2] = 3

	s := sum(a[:])
	fmt.Println(s);
	
	fmt.Println(len(a))
	
	m := map[string]int{"one":1, "two":2}
	m["ttt"] = 3
	
	fmt.Println(len(m))
	fmt.Println(m["two"])
	
	for key, value := range m {
		fmt.Println(key, "\t", m[key], "\t", value)
	}
	
	var m1 map[int]string
	m1 = make(map[int]string)
	m1[1] = "t1"
	
	for key, value := range m1 {
		fmt.Println(key, "\t", m1[key], "\t", value)
	}
*/

/*
	f, err := os.Open("./study.go")
	if f == nil {
		fmt.Fprintf(os.Stderr, "error %s\n", err)
		os.Exit(1) 
	}
	
	var buf [1024]byte
	for {
		switch nr, er := f.Read(buf[:]); true {
		case nr < 0:
			fmt.Fprintf(os.Stderr, "from %s\n", er)
			os.Exit(1)
		case nr == 0:
			os.Exit(1)
		case nr > 0:
			if nw, ew := os.Stdout.Write(buf[0:nr]); nw != nr {
				fmt.Fprintf(os.Stderr, "from1 %s\n", ew)
				os.Exit(1)
			}
		}
	}
	
	f.Close()
*/
/*
	var m map[string]int
	m = make(map[string]int)
	m["t1"] = 1
	m["t2"] = 2
	sum := 0
	for _, value := range m {
		sum += value
	}
	for key, _ := range m {
		sum += m[key]
	}
	fmt.Println(sum)
*/
	
/*
	sum := 0
	cnt := 0
	for cnt < 10 {
		sum += cnt
		cnt++
	}
	fmt.Println(sum)
*/

/*
	for pos, char := range "개발사항" {
		fmt.Printf("character %c starts at byte position %d\n", char, pos)	
	}
*/

/*
	for i := 0; i < 5; i++ {
		defer fmt.Printf("%d ", i)	
	}
*/
}


func ReadFull(r os.File, buf[]byte) (n int, err error) {
	for len(buf) > 0 && err == nil {
		var nr int
		nr, err = r.Read(buf)
		n += nr
		buf = buf[nr:len(buf)]
	}
	return
}

// defer : deferred function
func Contents(filename string) (name string, err error) {
	f, err := os.Open(filename)
	if err != nil {
		return "", err	
	}
	defer f.Close()
	
	var result []byte
	buf := make([]byte, 100)
	for {
		n, err := f.Read(buf[0:])
		result = append(result, buf[0:n]...)
		if err != nil {
			if err == io.EOF {
				break	
			}
			return "", err
		}	
	}
	return string(result), nil
}

func unhex(c byte) byte {
	switch {
	case '0' <= c && c <= '9':
		return c - '0'
	case 'b' <= c && c <= 'f':
		return c - 'a' + 10
	case 'A' <= c && c <= 'F':
		return c - 'A' + 10
	}
	return 0
}

/*
func nextInt(b []byte, i int) (int, int) {
	for ; i < len(b) && !isDigit(b[i]); i++ {
	}
	x := 0
	for ; i < len(b) && isDigit(b[i]); i++ {
		x = x * 10 + int(b[i]) - '0'
	}
	return x, i
}
*/

func sum(a []int) int {
	s := 0
	for i := 0; i < len(a); i++ {
		s += a[i]
	}
	return s
}

func append1(slice, data []byte) []byte {
	l := len(slice)
	if l + len(data) > cap(slice) {
		newSlice := make([]byte, (l+len(data))*2)
		copy(newSlice, slice)
		slice = newSlice
	}
	
	slice = slice[0:l+len(data)]
	for i, c := range data {
		slice[l+i] =c
	}
	
	return slice
}

func server(i int) {
	for {
		fmt.Println(i)
	}
}