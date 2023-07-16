// ICBMer - Go
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
package main

import (
	"fmt"
	"os"
)

func main() {
	if len(os.Args) == 1 {
		fmt.Println("Missing IBCM code file")
		return
	}

	ibcm := Executor{}
	ibcm = ibcm.WithLogging(true)

	ibcm, err := ibcm.FromFile(os.Args[1])
	if err != nil {
		fmt.Println(err)
		return
	}
	// ibcm.Print()

	err = ibcm.Run()
	if err != nil {
		fmt.Println(err)
	}
}
