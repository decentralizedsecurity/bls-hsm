package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
	"sync"

	"github.com/tarm/serial"
)

var mutex sync.Mutex

var v bool
var port string

func init() {
	flag.BoolVar(&v, "v", false, "display detailed output")
	flag.StringVar(&port, "port", "8080", "Socket port")
}

func Handler(conn net.Conn, s *serial.Port) {
	mutex.Lock()
	defer mutex.Unlock()
	defer conn.Close()

	buf := make([]byte, 65535)

	l := 0
	cl := 0
	readingBody := false
	var index int
	var index2 int
	for {
		n, err := conn.Read(buf[l:])
		if err != nil {
			break
		}
		l += n
		if !readingBody {
			if strings.Contains(string(buf), "\r\n\r\n") {
				if !strings.Contains(string(buf), "Content-Length:") {
					break
				} else {
					index = strings.Index(string(buf), "Content-Length:")
					index2 = strings.Index(string(buf), "\r\n\r\n")
					indexcl := strings.Index(string(buf[index:]), "\r\n")
					cl, err = strconv.Atoi(string(buf[index+16 : index+indexcl]))
					if cl == 0 {
						break
					}
					readingBody = true
					if l-cl == index2+4 {
						break
					}
				}
			}
		} else {
			if l-cl == index2+4 {
				break
			}
		}
	}
	fmt.Println("REQUEST")
	if v {
		fmt.Println(string(buf) + "\r\n")
	}

	n, err := s.Write(buf[:l])
	if err != nil {
		s.Close()
		log.Fatal(err)
	}
	_ = n

	scanner := bufio.NewScanner(s)
	scanner.Split(bufio.ScanRunes)
	var b bytes.Buffer

	l = 0
	cl = 0
	readingBody = false

	for scanner.Scan() {
		b.Write([]byte(scanner.Text()))
		l++
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
					if l-cl == index2+4 {
						break
					}
				}
			}
		} else {
			if l-cl == index2+4 {
				break
			}
		}
	}

	fmt.Println("RESPONSE")
	if v {
		fmt.Println(string(b.Bytes()) + "\r\n")
	}

	n, err = conn.Write(b.Bytes())

}

func main() {
	flag.Parse()
	if len(os.Args) == 2 || len(os.Args) == 3 || len(os.Args) == 4 {
		com := os.Args[len(os.Args)-1]

		c := &serial.Config{Name: com, Baud: 115200}
		s, err := serial.OpenPort(c)
		if err != nil {
			log.Fatal(err)
		}
		defer s.Close()
		fmt.Println("Connected to serial port " + com)

		l, err := net.Listen("tcp", "localhost:"+port)
		if err != nil {
			fmt.Println("Error listening:", err.Error())
			os.Exit(1)
		}
		defer l.Close()
		fmt.Println("Listening on port " + port)

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
