package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"bufio"
	"flag"
)


func main(){
	verb := flag.Bool("v", false, "display detailed output")
	flag.Parse()
	
	c := &serial.Config{Name: "COM4", Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil{
		log.Fatal(err)
	}

	str := make([]string, 10, 98)

	scanner := bufio.NewScanner(s)

	set_shell_params(s, scanner)

	fmt.Println("Deleting previously generated keys...")
	reset(s, scanner, *verb)
	fmt.Printf("\n\n")

	fmt.Println("Generating 10 keys...")
	keygen(s, scanner, str)
	fmt.Printf("\n\n")

	fmt.Println("Retrieving generated keys...")
	getkeys(s, scanner, *verb)
	fmt.Printf("\n\n")

	fmt.Println("Checking keys are different...")
	check(s, scanner, str)
	fmt.Printf("\n\n")

	fmt.Println("Attempting to generate extra key...")
	keygenext(s, scanner, *verb)
	fmt.Printf("\n\n")

	fmt.Println("Attempting to sign message with wrong length...")
	sign := ""
	msg := "565656565656565656565656565656565656565656565656565656565656565"
	signature(s, scanner, msg, &sign, str, *verb)
	fmt.Printf("\n\n")

	fmt.Println("Attempting to sign message with correct length...")
	msg = "5656565656565656565656565656565656565656565656565656565656565656"
	signature(s, scanner, msg, &sign, str, *verb)
	fmt.Printf("\n\n")

	fmt.Println("Attempting to verify generated signature...")
	verify(s, scanner, msg, sign, str, *verb)
	fmt.Printf("\n\n")

	fmt.Println("Deleting keys...")
	reset(s, scanner, *verb)
	fmt.Printf("\n\n")

	reset_shell_params(s)
}