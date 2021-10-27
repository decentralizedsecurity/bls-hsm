package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"time"

	"github.com/fatih/color"
	"github.com/tarm/serial"
)

var verb bool

func init() {
	flag.BoolVar(&verb, "v", false, "display detailed output")
}

func main() {
	flag.Parse()
	if len(os.Args) == 2 || len(os.Args) == 3 {
		com := os.Args[len(os.Args)-1]
		fmt.Println("Running tests...")

		c := &serial.Config{Name: com, Baud: 115200}
		s, err := serial.OpenPort(c)
		if err != nil {
			log.Fatal(err)
		}

		str := make([]string, 10, 98)

		scanner := bufio.NewScanner(s)

		set_shell_params(s, scanner)

		test := 0
		passed := make([]bool, 8)
		times := make([]time.Duration, 3)
		if verb {
			fmt.Println("Deleting previously generated keys...")
		}
		reset(s, scanner, verb, passed, test)
		test = 7
		if verb {
			fmt.Printf("\n\n")
		}

		if verb {
			fmt.Println("Generating 10 keys...")
		}
		keygen(s, scanner, str, verb, passed, times)
		if verb {
			fmt.Printf("\n\n")
		}

		if verb {
			fmt.Println("Retrieving generated keys...")
		}
		getkeys(s, scanner, verb, passed)
		if verb {
			fmt.Printf("\n\n")
		}

		if verb {
			fmt.Println("Checking keys are different...")
		}
		check(s, scanner, str, verb, passed)
		if verb {
			fmt.Printf("\n\n")
		}

		if verb {
			fmt.Println("Attempting to generate extra key...")
		}
		keygenext(s, scanner, verb, passed)
		if verb {
			fmt.Printf("\n\n")
		}

		sign := ""
		if verb {
			fmt.Println("Attempting to sign message...")
		}
		msg := "5656565656565656565656565656565656565656565656565656565656565656"
		signature(s, scanner, msg, &sign, str, verb, passed, times)
		if verb {
			fmt.Printf("\n\n")
		}

		if verb {
			fmt.Println("Attempting to verify generated signature...")
		}
		verify(s, scanner, msg, sign, str, verb, passed, times)
		if verb {
			fmt.Printf("\n\n")
		}

		if verb {
			fmt.Println("Deleting keys...")
		}
		reset(s, scanner, verb, passed, test)
		if verb {
			fmt.Printf("\n\n")
		}

		reset_shell_params(s)

		//pass := "\u2714"
		//fail := "\u274c"

		color.HiCyan("RESULTS:")
		fmt.Println("----------------------------------------")
		if verb {
			fmt.Printf("Delete previous keys..........")
			if passed[0] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Generate 10 keys..............")
			if passed[1] {
				color.HiGreen("PASSED")
				color.HiMagenta("%s elapsed", times[0])
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Retrieve generated keys.......")
			if passed[2] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Check keys are different......")
			if passed[3] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Try to generate extra key.....")
			if passed[4] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Sign msg......................")
			if passed[5] {
				color.HiGreen("PASSED")
				color.HiMagenta("%s elapsed", times[1])
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Verify signature..............")
			if passed[6] {
				color.HiGreen("PASSED")
				color.HiMagenta("%s elapsed", times[2])
			} else {
				color.Red("FAILED")
			}

			fmt.Printf("Delete keys...................")
			if passed[7] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}
		}
		cont := 0
		for i := 0; i < 8; i++ {
			if passed[i] {
				cont++
			}
		}
		color.HiMagenta("Total.........................%d/8", cont)
		fmt.Println("----------------------------------------")
	} else {
		fmt.Println("Usage: .\\test.exe [-v] COMport")
	}
}
