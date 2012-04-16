package main 

import (
	"fmt"
	"os"
	"io"
	"bufio"
)

func main() {
	var a, b, c uint64
	var value uint64
	var count int = 1
	in := "/Users/dark264sh/Downloads/A-large-practice.in.txt"
//	in := "/Users/dark264sh/Downloads/input.txt"
	out := "/Users/dark264sh/Downloads/R4.txt"
	
	i, e1 := os.Open(in)
	if e1 != nil {
		fmt.Fprintf(os.Stderr, "file=%s error=%s\n", in, e1)
		os.Exit(1)
	}
	defer i.Close()
	
	o, e2 := os.OpenFile(out, os.O_RDWR | os.O_CREATE | os.O_TRUNC, 0666)
	if e2 != nil {
		fmt.Fprintf(os.Stderr, "file=%s error=%s\n", out, e2)
		os.Exit(1)
	}
	defer o.Close()
	
	r := bufio.NewReader(i)
	for {
		line, e3 := r.ReadString('\n')
		if e3 == io.EOF {
			os.Exit(1)
		}
		
		cnt, e4 := fmt.Sscanf(line, "%d %d %d", &a, &b, &c)
		if e4 != nil || cnt != 3 {
			continue
		}
	
		value = calc(a, b, c)
		fmt.Fprintf(o, "Case #%d: %d\n", count, value)
		fmt.Fprintf(os.Stdout, "===> %d %d %d %d\n", a, b, c, value)	
		count++
		
//		fmt.Fprintf(os.Stdout, "test=%d %f\n", 3/2, math.Mod(3, 2))
	}
}

func calc(month, day, week uint64) uint64 {
	var base uint64 = 0
	var line uint64 = 0
	var i, j, k uint64
	
	for i = 0; i < month; i++ {
		for j = 0; j < day; j++ {
			base++
			if base == week {
				base = 0
				line++
			}
		}
		if base != 0 {
			line++
		} else {
			fmt.Fprintf(os.Stdout, "month=%d day=%d line=%d\n", i, j, line)
			
			re := month % (i + 1)
			line = month / (i + 1) * line
			
			for k = 0; k < re; k++ {
				for j = 0; j < day; j++ {
					base++
					if base == week {
						base = 0
						line++
					}
				}
				line++
			}
			break
		}
	}
	
	
	return line
}