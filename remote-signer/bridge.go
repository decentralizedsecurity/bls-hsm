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
	"syscall"
	"time"

	"github.com/tarm/serial"
)

var mutex sync.Mutex
var wg sync.WaitGroup
var logFile *os.File

var LOG_FILE string = "./log.txt"

var v bool
var port string
var d bool
var p string

func init() {
	flag.BoolVar(&v, "v", false, "display detailed output")
	flag.StringVar(&port, "port", "8080", "Socket port")
	flag.BoolVar(&d, "d", false, "log requests in log.txt")
	flag.StringVar(&p, "p", LOG_FILE, "Path to log file")
}

func Handler(conn net.Conn, s *serial.Port) {
	defer log.Println("Handler done")
	defer wg.Done()
	defer log.Println("Conn closed")
	defer conn.Close()

	ch0 := make(chan int, 1)

	go func() {
		mutex.Lock()
		ch0 <- 1
	}()

	select {
	case res := <-ch0:
		_ = res
	case <-time.After(time.Duration(3) * time.Second):
		log.Println("Timeout: Mutex couldn't be locked")
		return
	}
	defer log.Println("Mutex unlocked")
	defer mutex.Unlock()

	ch := make(chan int, 1)

	go func() {
		buf := make([]byte, 65535)
		defer func() {
			buf = nil
		}()

		l := 0
		cl := 0
		readingBody := false
		var index int
		var index2 int
		for {
			n, err := conn.Read(buf[l:])
			if err != nil {
				log.Println(err)
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

		log.Println(l, " bytes read")
		log.Println("REQUEST")
		log.Println(string(buf[:l]))

		fmt.Println("REQUEST")
		if v {
			fmt.Println(string(buf) + "\r\n")
		}

		n, err := s.Write(buf[:l])
		if err != nil {
			log.Println(err)
			ch <- 1
			return
		}
		log.Println(n, " bytes writen")

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

		log.Println(l, " bytes read")
		log.Println("RESPONSE")
		log.Println(string(b.Bytes()))

		fmt.Println("RESPONSE")
		if v {
			fmt.Println(string(b.Bytes()) + "\r\n")
		}

		n, err = conn.Write(b.Bytes())
		if err != nil {
			log.Println(err)
			ch <- 1
			return
		}
		ch <- 1
	}()

	select {
	case res := <-ch:
		_ = res
		return
	case <-time.After(time.Duration(3) * time.Second):
		log.Println("Timeout: Didn't receive response. Closing connection")
		return
	}
}

func main() {
	flag.Parse()
	if len(os.Args) >= 2 && len(os.Args) <= 6 {

		if d {
			logFile, err := os.OpenFile(p, os.O_APPEND|os.O_RDWR|os.O_CREATE, 0644)
			if err != nil {
				log.Println(err)
				return
			}
			log.SetOutput(logFile)
			log.SetFlags(log.Lshortfile | log.LstdFlags)
			log.Println("Log file opened")
			defer fmt.Println("Log file closed")
			defer logFile.Close()
			defer log.Println("Closing log file")
		}

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
			log.Panic("Error listening:", err.Error())
		}
		defer l.Close()
		fmt.Println("Listening on port " + port)

		// Go routine to restart the bridge periodically
		go func() {
			time.Sleep(600 * time.Second)
			mutex.Lock()
			log.Println("Restarting bridge...")
			args := os.Args
			syscall.Exec(args[0], args, os.Environ())
		}()

		for {
			conn, err := l.Accept()
			if err != nil {
				log.Panic("Error accepting: ", err.Error())
			}
			wg.Add(1)
			go Handler(conn, s)
		}
		wg.Wait()
	} else {
		fmt.Println("Usage: " + os.Args[0] + " COMport")
	}
}
