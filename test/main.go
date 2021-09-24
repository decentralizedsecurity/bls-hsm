package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"bufio"
	"flag"
	"os"
)


func main(){
	if len(os.Args) == 2{
		com := os.Args[1]
		fmt.Println("Running tests...")
		verb := flag.Bool("v", false, "display detailed output")
		flag.Parse()
	
		c := &serial.Config{Name: com, Baud: 115200}
		s, err := serial.OpenPort(c)
		if err != nil{
			log.Fatal(err)
		}

		str := make([]string, 10, 98)

		scanner := bufio.NewScanner(s)

		set_shell_params(s, scanner)

		test := 0
		passed := make([]bool, 9)
		if *verb{
			fmt.Println("Deleting previously generated keys...")
		}
		reset(s, scanner, *verb, passed, test)
		test = 8
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Generating 10 keys...")
		}
		keygen(s, scanner, str, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Retrieving generated keys...")
		}
		getkeys(s, scanner, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Checking keys are different...")
		}
		check(s, scanner, str, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Attempting to generate extra key...")
		}
		keygenext(s, scanner, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Attempting to sign message with wrong length...")
		}
		sign := ""
		msg := "565656565656565656565656565656565656565656565656565656565656565"
		signature(s, scanner, msg, &sign, str, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Attempting to sign message with correct length...")
		}
		msg = "5656565656565656565656565656565656565656565656565656565656565656"
		signature(s, scanner, msg, &sign, str, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Attempting to verify generated signature...")
		}
		verify(s, scanner, msg, sign, str, *verb, passed)
		if *verb{
			fmt.Printf("\n\n")
		}

		if *verb{
			fmt.Println("Deleting keys...")
		}
		reset(s, scanner, *verb, passed, test)
		if *verb{
			fmt.Printf("\n\n")
		}

		reset_shell_params(s)

		fmt.Println("RESULTS:")
		fmt.Println("----------------------------------------")
		fmt.Printf("Delete previous keys..........")
		if passed[0]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Generate 10 keys..............")
		if passed[1]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Retrieve generated keys.......")
		if passed[2]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Check keys are different......")
		if passed[3]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Try to generate extra key.....")
		if passed[4]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Sign msg with wrong length....")
		if passed[5]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Sign correct msg..............")
		if passed[6]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Verify signature..............")
		if passed[7]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}

		fmt.Printf("Delete keys...................")
		if passed[8]{
			fmt.Printf("PASSED\n")
		}else{
			fmt.Printf("FAILED\n")
		}
		fmt.Println("----------------------------------------")
	}else{
		fmt.Println("Usage: .\\test.exe COMport [-v]")
	}
}