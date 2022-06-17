package main

import (
	"bufio"
	"bytes"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"

	"github.com/tarm/serial"
)

var flag bool = false

func Handler(conn net.Conn, s *serial.Port) {
	for flag {
	}
	flag = true
	defer func() {
		flag = false
	}()
	defer conn.Close()

	buf := make([]byte, 65535)

	l := 0

	n, err := conn.Read(buf)
	l += n

	fmt.Println("REQUEST")
	fmt.Println(string(buf) + "\r\n")

	n, err = s.Write(buf[:l])
	if err != nil {
		s.Close()
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(s)
	scanner.Split(bufio.ScanRunes)
	var b bytes.Buffer

	cl := 0
	readingBody := false
	var index int
	var index2 int
	for scanner.Scan() {
		b.Write([]byte(scanner.Text()))
		if !readingBody {
			if strings.Contains(b.String(), "\r\n\r\n") {
				if !strings.Contains(b.String(), "Content-Length:") {
					break
				} else {
					index = strings.Index(b.String(), "Content-Length:")
					index2 = strings.Index(b.String(), "\r\n\r\n")
					cl, err = strconv.Atoi(string(b.Bytes()[index+16 : index2]))
					if cl == 0 {
						break
					}
					readingBody = true
				}
			}
		} else {
			if len(b.Bytes()[index2+4:])-cl == 0 {
				break
			}
		}
	}

	fmt.Println("RESPONSE")
	fmt.Println(string(b.Bytes()) + "\r\n")

	n, err = conn.Write(b.Bytes())

}

func main() {
	if len(os.Args) == 2 {
		com := os.Args[1]

		c := &serial.Config{Name: com, Baud: 115200}
		s, err := serial.OpenPort(c)
		if err != nil {
			log.Fatal(err)
		}
		defer s.Close()
		fmt.Println("Connected to serial port " + com)

		l, err := net.Listen("tcp", "localhost:8080")
		if err != nil {
			fmt.Println("Error listening:", err.Error())
			os.Exit(1)
		}
		defer l.Close()
		fmt.Println("Listening on port 8080")

		for {
			conn, err := l.Accept()
			if err != nil {
				fmt.Println("Error accepting: ", err.Error())
				os.Exit(1)
			}
			go Handler(conn, s)
		}
	} else {
		fmt.Println("Usage: " + os.Args[0] + " COMport")
	}
}
