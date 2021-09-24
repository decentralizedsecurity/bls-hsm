package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"bufio"
	"strings"
	"time"
	"github.com/fatih/color"
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

func keygen(s *serial.Port, scanner *bufio.Scanner, str []string, verb bool, passed []bool){

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
	if strings.Contains(str[9], "0x"){
		passed[1] = true
		if verb{
			elapsed := time.Since(t)
			fmt.Printf("%s elapsed\n", elapsed)
			color.HiGreen("Ok")
		}
	}else{
		passed[1] = false
		if verb{
			color.Red("Failed")
		}
	}
}

func getkeys(s *serial.Port, scanner *bufio.Scanner, verb bool, passed []bool){
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
			passed[2] = true
			if verb{
				color.HiGreen("Ok")
			}
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), "}"){
		passed[2] = false
		if verb{
			color.Red("Failed")
		}
	}
}
func check(s *serial.Port, scanner *bufio.Scanner, str []string, verb bool, passed []bool){

	error := false
	for i := 0; i < 9; i++{
		for j := i+1; j < 10; j++{
			if str[i] == str[j]{
				error = true
			}
		}
	}
	if error{
		passed[3] = false
		if verb{
			color.Red("Failed")
		}
	}else{
		passed[3] = true
		if verb{
			color.HiGreen("Ok")
		}
	}
}
func keygenext(s *serial.Port, scanner *bufio.Scanner, verb bool, passed []bool){
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
			passed[4] = true
			if verb{
				color.HiGreen("Ok")
			}
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), "."){
		passed[4] = false
		if verb{
			color.Red("Failed")
		}
	}
}
func signature(s *serial.Port, scanner *bufio.Scanner, msg string, sign *string, str []string, verb bool, passed []bool){
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
			passed[5] = true
			if verb{
				color.HiGreen("Ok")
			}
			break
		}
		if strings.Contains(scanner.Text(), "0x"){
			passed[6] = true
			if verb{
				elapsed := time.Since(t)
				fmt.Printf("%s elapsed\n", elapsed)
				color.HiGreen("Ok")
			}
			*sign = scanner.Text()
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), ".") && len(msg) != 64{
		passed[5] = false
		if verb{
			color.Red("Failed")
		}
	}
	if !strings.Contains(scanner.Text(), "0x") && len(msg) == 64{
		passed[6] = false
		if verb{
			color.Red("Failed")
		}
	}
}
func verify(s *serial.Port, scanner *bufio.Scanner, msg string, sign string, str []string, verb bool, passed []bool){
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
			passed[7] = true
			if verb{
				elapsed := time.Since(t)
				fmt.Printf("%s elapsed\n", elapsed)
				color.HiGreen("Ok")
			}
			break
		}
		if strings.Contains(scanner.Text(), "Error"){
			passed[7] = false
			if verb{
				color.Red("Failed")
			}
			break
		}
	}
}
func reset(s *serial.Port, scanner *bufio.Scanner, verb bool, passed []bool, test int){
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
			passed[test] = true
			if verb{
				color.HiGreen("Ok")
			}
			break
		}
	}
	if !strings.Contains(scanner.Text(), "deleted"){
		passed[test] = false
		if verb{
			color.Red("Failed")
		}
	}
}