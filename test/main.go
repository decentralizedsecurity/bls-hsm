package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"time"
	"bufio"
	"strings"
)

func main(){
	c := &serial.Config{Name: "COM4", Baud: 115200, ReadTimeout: time.Second * 1}
	s, err := serial.OpenPort(c)
	if err != nil{
		log.Fatal(err)
	}

	str := make([]string, 10, 98)

	fmt.Println("Generating 10 keys...")
	for i := 0; i < 10; i++{
		n, err := s.Write([]byte("keygen\n"))
		if err != nil{
			log.Fatal(err)
		}

		_ = n

		scanner := bufio.NewScanner(s)
		for scanner.Scan(){
			if strings.HasPrefix(scanner.Text(), "0x"){
				str[i] = scanner.Text()
				break
			}
		}

	}
	fmt.Printf("\n")

	fmt.Println("Retrieving generated keys...")
	n, err := s.Write([]byte("getkeys\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n

	scanner := bufio.NewScanner(s)
	for scanner.Scan(){
		if !strings.Contains(scanner.Text(), "uart"){
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), "}"){
			break
		}
	}
	fmt.Printf("\n")

	fmt.Println("Checking keys are different...")
	if str[0] != str[1] && str[0] != str[2] && str[0] != str[3] && str[0] != str[4] && str[0] != str[5] && str[0] != str[6] && str[0] != str[7] && str[0] != str[8] && str[0] != str[9] && 
	str[1] != str[2] && str[1] != str[3] && str[1] != str[4] && str[1] != str[5] && str[1] != str[6] && str[1] != str[7] && str[1] != str[8] && str[1] != str[9] && 
	str[2] != str[3] && str[2] != str[4] && str[2] != str[5] && str[2] != str[6] && str[2] != str[7] && str[2] != str[8] && str[2] != str[9] && 
	str[3] != str[4] && str[3] != str[5] && str[3] != str[6] && str[3] != str[7] && str[3] != str[8] && str[3] != str[9] && 
	str[4] != str[5] && str[4] != str[6] && str[4] != str[7] && str[4] != str[8] && str[4] != str[9] && 
	str[5] != str[6] && str[5] != str[7] && str[5] != str[8] && str[5] != str[9] && 
	str[6] != str[7] && str[6] != str[8] && str[6] != str[9] && 
	str[7] != str[8] && str[7] != str[9] && 
	str[8] != str[9]{
		fmt.Println("Ok")
	}else{
		fmt.Println("Error")
	}
	fmt.Printf("\n")

	fmt.Println("Attempting to generate extra key...")
	n, err = s.Write([]byte("keygen\n"))
	if err != nil{
		log.Fatal(err)
	}

	for scanner.Scan(){
		if !strings.Contains(scanner.Text(), "uart"){
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), "."){
			break
		}
	}
	fmt.Printf("\n")

	fmt.Println("Attempting to sign message with incorrect length...")
	msg := "565656565656565656565656565656565656565656565656565656565656565"
	
	fmt.Println("Public key: " + str[0])
	fmt.Println("Message: " + msg)

	n, err = s.Write([]byte("signature " + str[0] + " " + msg + "\n"))
	if err != nil{
		log.Fatal(err)
	}

	for scanner.Scan(){
		if strings.HasSuffix(scanner.Text(), "."){
			fmt.Println(scanner.Text())
			break
		}
	}
	fmt.Printf("\n")

	fmt.Println("Attempting to sign message with correct length...")
	msg = "5656565656565656565656565656565656565656565656565656565656565656"
	
	fmt.Println("Public key: " + str[0])
	fmt.Println("Message: " + msg)

	n, err = s.Write([]byte("signature " + str[0] + " " + msg + "\n"))
	if err != nil{
		log.Fatal(err)
	}

	sign := ""

	for scanner.Scan(){
		if strings.Contains(scanner.Text(), "Signature"){
			fmt.Println(scanner.Text())
			scanner.Scan()
			sign = scanner.Text()
			fmt.Println(sign)
			break
		}
	}
	fmt.Printf("\n")

	fmt.Println("Attempting to verify generated signature...")

	n, err = s.Write([]byte("verify " + str[0] + " " + msg + " " + sign + "\n"))
	if err != nil{
		log.Fatal(err)
	}

	for scanner.Scan(){
		if strings.Contains(scanner.Text(), "Success") || strings.Contains(scanner.Text(), "Error"){
			fmt.Println(scanner.Text())
			break
		}
	}
	fmt.Printf("\n")

	fmt.Println("Deleting keys...")

	n, err = s.Write([]byte("reset\n"))
	if err != nil{
		log.Fatal(err)
	}

	for scanner.Scan(){
		if strings.Contains(scanner.Text(), "deleted"){
			fmt.Println(scanner.Text())
			break
		}
	}
	fmt.Printf("\n")
}