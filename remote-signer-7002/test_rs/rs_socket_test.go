package socket_test

import (
	"bytes"
	"net/http"
	"os"
	"testing"
)

func TestUpcheck(t *testing.T) {
	ip := "192.168.1.40"

	// Create a new HTTP request
	req, err := http.NewRequest("GET", "http://"+ip+":4242/upcheck", nil)
	if err != nil {
		t.Fatalf("Could not create request: %v", err)
	}
	req.Close = true
	// Create a new HTTP client and send the request
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		t.Fatalf("Could not send request: %v", err)
	}
	defer resp.Body.Close()

	// Check the response
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("Expected status OK; got %v", resp.Status)
	}
}

func TestImportKeystore(t *testing.T) {
	ip := "192.168.1.40"

	keystore, err := os.ReadFile("../../samples/keystores/keystore_request_weak.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte(string(keystore[:]))

	reader := bytes.NewReader(request)

	// Create a new HTTP request
	req, err := http.NewRequest("POST", "http://"+ip+":4242/eth/v1/keystores", reader)
	if err != nil {
		t.Fatalf("Could not create request: %v", err)
	}
	req.Header.Set("Content-Type", "application/json")
	req.Close = true

	t.Log(req.Body)

	// Create a new HTTP client and send the request
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		t.Fatalf("Could not send request: %v", err)
	}
	defer resp.Body.Close()

	// Check the response
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("Expected status OK; got %v", resp.Status)
	}
}

func TestSign(t *testing.T) {
	ip := "192.168.1.40"

	block, err := os.ReadFile("../../samples/artifacts/aggregate_and_proof.json")
	if err != nil {
		t.Error("Failed reading request body")
	}

	request := []byte(string(block[:]))

	reader := bytes.NewReader(request)

	// Create a new HTTP request
	req, err := http.NewRequest("POST", "http://"+ip+":4242/api/v1/eth2/sign/0xae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2", reader)
	if err != nil {
		t.Fatalf("Could not create request: %v", err)
	}
	req.Header.Set("Content-Type", "application/json")
	req.Close = true

	t.Log(req.Body)

	// Create a new HTTP client and send the request
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		t.Fatalf("Could not send request: %v", err)
	}
	defer resp.Body.Close()

	// Check the response
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("Expected status OK; got %v", resp.Status)
	}
}
