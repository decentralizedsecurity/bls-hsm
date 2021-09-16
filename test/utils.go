package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"bufio"
	"strings"
)

func set_shell_params(s *serial.Port){
	n, err := s.Write([]byte("shell colors off\n"))
	if err != nil{
		log.Fatal(err)
	}


	n, err = s.Write([]byte("prompt off\n"))
	if err != nil{
		log.Fatal(err)
	}


	n, err = s.Write([]byte("shell echo off\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
}

func reset_shell_params(s *serial.Port){
	n, err := s.Write([]byte("shell colors on\n"))
	if err != nil{
		log.Fatal(err)
	}


	n, err = s.Write([]byte("prompt on\n"))
	if err != nil{
		log.Fatal(err)
	}


	n, err = s.Write([]byte("shell echo on\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
}

func keygen(s *serial.Port, scanner *bufio.Scanner, str []string){
	for i := 0; i < 10; i++{
		n, err := s.Write([]byte("keygen\n"))
		if err != nil{
			log.Fatal(err)
		}
		_ = n

		for scanner.Scan(){
			if strings.HasPrefix(scanner.Text(), "0x"){
				str[i] = scanner.Text()
				break
			}
		}
	}
}

func getkeys(s *serial.Port, scanner *bufio.Scanner){
	n, err := s.Write([]byte("getkeys\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
	
	for scanner.Scan(){
		fmt.Println(scanner.Text())
		if strings.HasSuffix(scanner.Text(), "}"){
			break
		}
	}
}
func check(s *serial.Port, scanner *bufio.Scanner, str []string){
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
}
func keygenext(s *serial.Port, scanner *bufio.Scanner){
	n, err := s.Write([]byte("keygen\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
	for scanner.Scan(){
		fmt.Println(scanner.Text())
		if strings.HasSuffix(scanner.Text(), "."){
			break
		}
	}
}
func signature(s *serial.Port, scanner *bufio.Scanner, msg string, sign *string, str []string){
	
	fmt.Println("Public key: " + str[0])
	fmt.Println("Message: " + msg)

	n, err := s.Write([]byte("signature " + str[0] + " " + msg + "\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n

	for scanner.Scan(){
		fmt.Println(scanner.Text())
		if strings.HasSuffix(scanner.Text(), "."){
			break
		}
		if strings.Contains(scanner.Text(), "0x"){
			*sign = scanner.Text()
			break
		}
	}
}
func verify(s *serial.Port, scanner *bufio.Scanner, msg string, sign string, str []string){
	n, err := s.Write([]byte("verify " + str[0] + " " + msg + " " + sign + "\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n

	for scanner.Scan(){
		fmt.Println(scanner.Text())
		if strings.Contains(scanner.Text(), "Success") || strings.Contains(scanner.Text(), "Error"){
			break
		}
	}
}
func reset(s *serial.Port, scanner *bufio.Scanner){
	n, err := s.Write([]byte("reset\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n

	for scanner.Scan(){
		fmt.Println(scanner.Text())
		if strings.Contains(scanner.Text(), "deleted"){
			break
		}
	}
}