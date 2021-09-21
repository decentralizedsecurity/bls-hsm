package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"bufio"
	"strings"
	"time"
)

func set_shell_params(s *serial.Port, scanner *bufio.Scanner){
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

	for scanner.Scan(){
		if strings.Contains(scanner.Text(), "echo"){
			break
		}
	}
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
	t := time.Now()
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
	elapsed := time.Since(t)
	fmt.Printf("%s elapsed\n", elapsed)
	if strings.Contains(str[9], "0x"){
		fmt.Println("Ok")
	}else{
		fmt.Println("Failed")
	}
}

func getkeys(s *serial.Port, scanner *bufio.Scanner, verb bool){
	n, err := s.Write([]byte("getkeys\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
	
	for scanner.Scan(){
		if verb{
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), "}"){
			fmt.Println("Ok")
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), "}"){
		fmt.Println("Failed")
	}
}
func check(s *serial.Port, scanner *bufio.Scanner, str []string){

	error := false
	for i := 0; i < 9; i++{
		for j := i+1; j < 10; j++{
			if str[i] == str[j]{
				error = true
			}
		}
	}
	if error{
		fmt.Println("Failed")
	}else{
		fmt.Println("Ok")
	}
}
func keygenext(s *serial.Port, scanner *bufio.Scanner, verb bool){
	n, err := s.Write([]byte("keygen\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
	for scanner.Scan(){
		if verb{
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), "."){
			fmt.Println("Ok")
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), "."){
		fmt.Println("Failed")
	}
}
func signature(s *serial.Port, scanner *bufio.Scanner, msg string, sign *string, str []string, verb bool){
	if verb{
		fmt.Println("Public key: " + str[0])
		fmt.Println("Message: " + msg)
	}
	n, err := s.Write([]byte("signature " + str[0] + " " + msg + "\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
	t := time.Now()

	for scanner.Scan(){
		if verb{
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), "."){
			fmt.Println("Ok")
			break
		}
		if strings.Contains(scanner.Text(), "0x"){
			elapsed := time.Since(t)
			fmt.Printf("%s elapsed\n", elapsed)
			fmt.Println("Ok")
			*sign = scanner.Text()
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), ".") && !strings.Contains(scanner.Text(), "0x"){
		fmt.Println("Failed")
	}
}
func verify(s *serial.Port, scanner *bufio.Scanner, msg string, sign string, str []string, verb bool){
	n, err := s.Write([]byte("verify " + str[0] + " " + msg + " " + sign + "\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n
	t := time.Now()

	for scanner.Scan(){
		if verb{
			fmt.Println(scanner.Text())
		}
		if strings.Contains(scanner.Text(), "Success"){
			elapsed := time.Since(t)
			fmt.Printf("%s elapsed\n", elapsed)
			fmt.Println("Ok")
			break
		}
		if strings.Contains(scanner.Text(), "Error"){
			fmt.Println("Failed")
			break
		}
	}
}
func reset(s *serial.Port, scanner *bufio.Scanner, verb bool){
	n, err := s.Write([]byte("reset\n"))
	if err != nil{
		log.Fatal(err)
	}
	_ = n

	for scanner.Scan(){
		if verb{
			fmt.Println(scanner.Text())
		}
		if strings.Contains(scanner.Text(), "deleted"){
			fmt.Println("Ok")
			break
		}
	}
	if !strings.Contains(scanner.Text(), "deleted"){
		fmt.Println("Failed")
	}
}