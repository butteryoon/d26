package main

import (
    "flag"
    "fmt"
    "strconv"
)

func main() {
    s := flag.Arg(0)
    i, err := strconv.Atoi(s)
    if err != nil {
        // handle error
        fmt.Println(s, err)
        return
    }
    fmt.Println(s, i)
}

