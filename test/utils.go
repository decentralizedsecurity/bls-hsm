package main

import (
	"bufio"
	"fmt"
	"log"
	"strings"
	"time"

	"github.com/fatih/color"
	"github.com/tarm/serial"
)

func set_shell_params(s *serial.Port, scanner *bufio.Scanner) {
	n, err := s.Write([]byte("shell colors off\n"))
	if err != nil {
		log.Fatal(err)
	}

	n, err = s.Write([]byte("prompt off\n"))
	if err != nil {
		log.Fatal(err)
	}

	n, err = s.Write([]byte("shell echo off\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n

	for scanner.Scan() {
		if strings.Contains(scanner.Text(), "echo") {
			break
		}
	}
}

func reset_shell_params(s *serial.Port) {
	n, err := s.Write([]byte("shell colors on\n"))
	if err != nil {
		log.Fatal(err)
	}

	n, err = s.Write([]byte("prompt on\n"))
	if err != nil {
		log.Fatal(err)
	}

	n, err = s.Write([]byte("shell echo on\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n
}

func keygen(s *serial.Port, scanner *bufio.Scanner, str []string, verb bool, passed []bool, times []time.Duration) {

	if !verb {
		fmt.Printf("Generate 10 keys..............")
	}

	t := time.Now()
	for i := 0; i < 10; i++ {
		n, err := s.Write([]byte("keygen\n"))
		if err != nil {
			log.Fatal(err)
		}
		_ = n

		for scanner.Scan() {
			if strings.HasPrefix(scanner.Text(), "0x") {
				str[i] = scanner.Text()
				break
			}
		}
	}
	if strings.Contains(str[9], "0x") {
		passed[1] = true
		times[0] = time.Since(t)
		if verb {
			color.HiMagenta("%s elapsed\n", times[0])
			color.HiGreen("PASSED")
		}
	} else {
		passed[1] = false
		if verb {
			color.Red("FAILED")
		}
	}

	if !verb {
		if passed[1] {
			color.HiGreen("PASSED")
			color.HiMagenta("%s elapsed\n", times[0])
		} else {
			color.Red("FAILED")
		}
	}
}

func getkeys(s *serial.Port, scanner *bufio.Scanner, verb bool, passed []bool) {
	n, err := s.Write([]byte("getkeys\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n

	if !verb {
		fmt.Printf("Retrieve generated keys.......")
	}

	for scanner.Scan() {
		if verb {
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), "}") {
			passed[2] = true
			if verb {
				color.HiGreen("PASSED")
			}
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), "}") {
		passed[2] = false
		if verb {
			color.Red("FAILED")
		}
	}

	if !verb {
		if passed[2] {
			color.HiGreen("PASSED")
		} else {
			color.Red("FAILED")
		}
	}
}
func check(s *serial.Port, scanner *bufio.Scanner, str []string, verb bool, passed []bool) {

	if !verb {
		fmt.Printf("Check keys are different......")
	}

	error := false
	for i := 0; i < 9; i++ {
		for j := i + 1; j < 10; j++ {
			if str[i] == str[j] {
				error = true
			}
		}
	}
	if error {
		passed[3] = false
		if verb {
			color.Red("FAILED")
		}
	} else {
		passed[3] = true
		if verb {
			color.HiGreen("PASSED")
		}
	}

	if !verb {
		if passed[3] {
			color.HiGreen("PASSED")
		} else {
			color.Red("FAILED")
		}
	}
}
func keygenext(s *serial.Port, scanner *bufio.Scanner, verb bool, passed []bool) {
	n, err := s.Write([]byte("keygen\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n

	if !verb {
		fmt.Printf("Try to generate extra key.....")
	}

	for scanner.Scan() {
		if verb {
			fmt.Println(scanner.Text())
		}
		if strings.HasSuffix(scanner.Text(), ".") {
			passed[4] = true
			if verb {
				color.HiGreen("PASSED")
			}
			break
		}
	}
	if !strings.HasSuffix(scanner.Text(), ".") {
		passed[4] = false
		if verb {
			color.Red("FAILED")
		}
	}

	if !verb {
		if passed[4] {
			color.HiGreen("PASSED")
		} else {
			color.Red("FAILED")
		}
	}
}
func signature(s *serial.Port, scanner *bufio.Scanner, msg string, sign *string, str []string, verb bool, passed []bool, times []time.Duration) {
	if verb {
		fmt.Println("Public key: " + str[0])
		fmt.Println("Message: " + msg)
	}
	n, err := s.Write([]byte("signature " + str[0] + " " + msg + "\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n

	if !verb {
		fmt.Printf("Sign msg......................")
	}

	t := time.Now()

	for scanner.Scan() {
		if verb {
			fmt.Println(scanner.Text())
		}
		if strings.Contains(scanner.Text(), "0x") {
			passed[5] = true
			times[1] = time.Since(t)
			if verb {
				color.HiMagenta("%s elapsed\n", times[1])
				color.HiGreen("PASSED")
			}
			*sign = scanner.Text()
			break
		}
	}
	if !strings.Contains(scanner.Text(), "0x") {
		passed[5] = false
		if verb {
			color.Red("FAILED")
		}
	}

	if !verb {
		if passed[5] {
			color.HiGreen("PASSED")
		} else {
			color.Red("FAILED")
		}
	}
}
func verify(s *serial.Port, scanner *bufio.Scanner, msg string, sign string, str []string, verb bool, passed []bool, times []time.Duration) {
	n, err := s.Write([]byte("verify " + str[0] + " " + msg + " " + sign + "\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n

	if !verb {
		fmt.Printf("Verify signature..............")
	}

	t := time.Now()

	for scanner.Scan() {
		if verb {
			fmt.Println(scanner.Text())
		}
		if strings.Contains(scanner.Text(), "Success") {
			passed[6] = true
			times[2] = time.Since(t)
			if verb {
				color.HiMagenta("%s elapsed\n", times[2])
				color.HiGreen("PASSED")
			}
			break
		}
		if strings.Contains(scanner.Text(), "Error") {
			passed[6] = false
			if verb {
				color.Red("FAILED")
			}
			break
		}
	}

	if !verb {
		if passed[6] {
			color.HiGreen("PASSED")
			color.HiMagenta("%s elapsed\n", times[2])
		} else {
			color.Red("FAILED")
		}
	}
}
func reset(s *serial.Port, scanner *bufio.Scanner, verb bool, passed []bool, test int) {
	n, err := s.Write([]byte("reset\n"))
	if err != nil {
		log.Fatal(err)
	}
	_ = n

	if !verb {
		if test == 0 {
			fmt.Printf("Delete previous keys..........")
		} else {
			fmt.Printf("Delete keys...................")
		}
	}

	for scanner.Scan() {
		if verb {
			fmt.Println(scanner.Text())
		}
		if strings.Contains(scanner.Text(), "deleted") {
			passed[test] = true
			if verb {
				color.HiGreen("PASSED")
			}
			break
		}
	}
	if !strings.Contains(scanner.Text(), "deleted") {
		passed[test] = false
		if verb {
			color.Red("FAILED")
		}
	}

	if !verb {
		if test == 0 {
			if passed[0] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}
		} else {
			if passed[7] {
				color.HiGreen("PASSED")
			} else {
				color.Red("FAILED")
			}
		}
	}
}
