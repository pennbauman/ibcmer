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
		fmt.Fprintln(os.Stderr, "Error: A code file must be provided")
		return
	}

	ibcm := Executor{}
	ibcm = ibcm.WithLogging(true)

	ibcm, err := ibcm.FromFile(os.Args[1])
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error:", err)
		return
	}
	// ibcm.Print()

	err = ibcm.Run()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Error:", err)
	}
}
