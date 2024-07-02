package main

import (
	"bufio"
	"flag"

	"github.com/tarm/serial"

	"os"
	"strings"
	"testing"
	"time"
)

var com = flag.String("com", "", "Serial port")
var c *serial.Config
var s *serial.Port

func TestImportKeystore(t *testing.T) {
	c = &serial.Config{Name: *com, Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		t.Error("Failed opening serial port")
	}
	_ = s
	defer s.Close()

	keystore, err := os.ReadFile("../../samples/eip2335/keystore_request.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte("POST /eth/v1/keystores HTTP/1.1\r\n" +
		"Host: localhost:80\r\n" +
		"Content-Type: application/json\r\n" +
		"Content-Length: 1212\r\n\r\n" + string(keystore[:]))

	scanner := bufio.NewScanner(s)

	n, err := s.Write(request)
	if err != nil {
		t.Error("Failed sending request")
	}
	_ = n

	ch := make(chan string, 1)
	go func() {
		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "200 Success response") {
				ch <- ""
				break
			} else if strings.Contains(scanner.Text(), "400 Bad request") {
				t.Error("Failed importing keystore")
				ch <- ""
				break
			}
		}
	}()
	select {
	case res := <-ch:
		_ = res
	case <-time.After(90 * time.Second):
		t.Error("Timeout")
	}
}

func TestSignBlock(t *testing.T) {
	c = &serial.Config{Name: *com, Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		t.Error("Failed opening serial port")
	}
	_ = s
	defer s.Close()

	block, err := os.ReadFile("../../samples/block.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte("POST /api/v1/eth2/sign/0xae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2 HTTP/1.1\r\n" +
		"Host: localhost:80\r\n" +
		"Content-Type: application/json\r\n" +
		"Content-Length: 1295\r\n\r\n" + string(block[:]))

	scanner := bufio.NewScanner(s)

	n, err := s.Write(request)
	if err != nil {
		t.Error("Failed sending request")
	}
	_ = n

	ch := make(chan string, 1)
	go func() {
		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "200 OK") {
				ch <- ""
				break
			} else if strings.Contains(scanner.Text(), "404") {
				t.Error("Failed")
				ch <- ""
				break
			}
		}
	}()
	select {
	case res := <-ch:
		_ = res
	case <-time.After(3 * time.Second):
		t.Error("Timeout")
	}
}

func TestSignAggregateAndProof(t *testing.T) {
	c = &serial.Config{Name: *com, Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		t.Error("Failed opening serial port")
	}
	_ = s
	defer s.Close()

	block, err := os.ReadFile("../../samples/aggregate_and_proof.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte("POST /api/v1/eth2/sign/0xae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2 HTTP/1.1\r\n" +
		"Host: localhost:80\r\n" +
		"Content-Type: application/json\r\n" +
		"Content-Length: 1636\r\n\r\n" + string(block[:]))

	scanner := bufio.NewScanner(s)

	n, err := s.Write(request)
	if err != nil {
		t.Error("Failed sending request")
	}
	_ = n

	ch := make(chan string, 1)
	go func() {
		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "200 OK") {
				ch <- ""
				break
			} else if strings.Contains(scanner.Text(), "404") {
				t.Error("Failed")
				ch <- ""
				break
			}
		}
	}()
	select {
	case res := <-ch:
		_ = res
	case <-time.After(3 * time.Second):
		t.Error("Timeout")
	}
}

func TestSignAggregationSlot(t *testing.T) {
	c = &serial.Config{Name: *com, Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		t.Error("Failed opening serial port")
	}
	_ = s
	defer s.Close()

	block, err := os.ReadFile("../../samples/aggregation_slot.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte("POST /api/v1/eth2/sign/0xae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2 HTTP/1.1\r\n" +
		"Host: localhost:80\r\n" +
		"Content-Type: application/json\r\n" +
		"Content-Length: 475\r\n\r\n" + string(block[:]))

	scanner := bufio.NewScanner(s)

	n, err := s.Write(request)
	if err != nil {
		t.Error("Failed sending request")
	}
	_ = n

	ch := make(chan string, 1)
	go func() {
		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "200 OK") {
				ch <- ""
				break
			} else if strings.Contains(scanner.Text(), "404") {
				t.Error("Failed")
				ch <- ""
				break
			}
		}
	}()
	select {
	case res := <-ch:
		_ = res
	case <-time.After(3 * time.Second):
		t.Error("Timeout")
	}
}

func TestSignAttestation(t *testing.T) {
	c = &serial.Config{Name: *com, Baud: 115200}
	s, err := serial.OpenPort(c)
	if err != nil {
		t.Error("Failed opening serial port")
	}
	_ = s
	defer s.Close()

	block, err := os.ReadFile("../../samples/attestation.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte("POST /api/v1/eth2/sign/0xae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2 HTTP/1.1\r\n" +
		"Host: localhost:80\r\n" +
		"Content-Type: application/json\r\n" +
		"Content-Length: 897\r\n\r\n" + string(block[:]))

	scanner := bufio.NewScanner(s)

	n, err := s.Write(request)
	if err != nil {
		t.Error("Failed sending request")
	}
	_ = n

	ch := make(chan string, 1)
	go func() {
		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "200 OK") {
				ch <- ""
				break
			} else if strings.Contains(scanner.Text(), "404") {
				t.Error("Failed")
				ch <- ""
				break
			}
		}
	}()
	select {
	case res := <-ch:
		_ = res
	case <-time.After(3 * time.Second):
		t.Error("Timeout")
	}
}
