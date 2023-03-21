package main

import (
	"bufio"
	"bytes"
	"encoding/hex"
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strings"

	"github.com/protolambda/go-keystorev4"
	"github.com/protolambda/zrnt/eth2/beacon/common"
	"github.com/protolambda/zrnt/eth2/beacon/phase0"
	"github.com/protolambda/zrnt/eth2/configs"
	"github.com/protolambda/ztyp/tree"
	"github.com/prysmaticlabs/prysm/shared/keystore"
	"github.com/tarm/serial"
)

var sk string
var pk string
var pkhex string
var v bool = false

//Compatible with Prysm
func decryptWeb3() error {
	ks := new(keystore.Keystore)
	keys, err := ks.GetKey(os.Args[2], os.Args[3])
	if err == nil {
		sk = hex.EncodeToString(keys.SecretKey.Marshal())
		pk = hex.EncodeToString(keys.PublicKey.Marshal())
		pkhex = "0x" + pk
	}
	return err
}

//Compatible with Web3Signer
func decryptEIP2335(ksjson *[]byte) error {
	var ks keystorev4.Keystore
	err := json.Unmarshal(*ksjson, &ks)
	skbin, err := ks.Decrypt([]byte(os.Args[3]))
	sk = hex.EncodeToString(skbin)
	pk = hex.EncodeToString(ks.Pubkey)
	pkhex = "0x" + pk
	return err
}

func getSigningRoot(bod []byte, supported *bool, signingroot *[]byte) error {
	var htr common.Root
	var domtype common.BLSDomainType
	var sr common.Root
	var bodymap map[string]interface{}
	err := json.Unmarshal(bod, &bodymap)
	if err != nil {
		return err
	}

	switch bodymap["type"].(string) {
	case "block", "BLOCK":
		*supported = true
		body, err := json.Marshal(bodymap["block"])
		if err != nil {
			return err
		}

		beaconblock := new(phase0.BeaconBlock)
		err = json.Unmarshal(body, beaconblock)
		if err != nil {
			return err
		}
		sp := configs.Mainnet
		htr = beaconblock.HashTreeRoot(sp, tree.GetHashFn())
		domtype = common.DOMAIN_BEACON_PROPOSER
		break
	case "attestation", "ATTESTATION":
		*supported = true
		body, err := json.Marshal(bodymap["attestation"])
		if err != nil {
			return err
		}
		attestation := new(phase0.AttestationData)
		err = json.Unmarshal(body, attestation)
		if err != nil {
			return err
		}
		htr = attestation.HashTreeRoot(tree.GetHashFn())
		domtype = common.DOMAIN_BEACON_ATTESTER
		break
	case "aggregation_slot", "AGGREGATION_SLOT":
		*supported = true
		body, err := json.Marshal(bodymap["aggregation_slot"])
		if err != nil {
			return err
		}
		type AggregationSlot struct {
			Slot common.Slot `json:slot`
		}
		agslot := new(AggregationSlot)
		err = json.Unmarshal(body, agslot)
		if err != nil {
			return err
		}
		htr = agslot.Slot.HashTreeRoot(tree.GetHashFn())
		domtype = common.DOMAIN_SELECTION_PROOF
		break
	case "aggregate_and_proof", "AGGREGATE_AND_PROOF":
		*supported = true
		body, err := json.Marshal(bodymap["aggregate_and_proof"])
		if err != nil {
			return err
		}
		agandproof := new(phase0.AggregateAndProof)
		err = json.Unmarshal(body, agandproof)
		if err != nil {
			return err
		}
		sp := configs.Mainnet
		htr = agandproof.HashTreeRoot(sp, tree.GetHashFn())
		domtype = common.DOMAIN_AGGREGATE_AND_PROOF
		break
	}

	if *supported {
		forkinfo, err := json.Marshal(bodymap["fork_info"])
		if err != nil {
			return err
		}
		var forkinfomap map[string]interface{}
		err = json.Unmarshal(forkinfo, &forkinfomap)
		if err != nil {
			return err
		}
		fork := new(common.Fork)
		forkbin, err := json.Marshal(forkinfomap["fork"])
		err = json.Unmarshal(forkbin, fork)
		if err != nil {
			return err
		}

		gvroot, err := hex.DecodeString(forkinfomap["genesis_validators_root"].(string)[2:])
		if err != nil {
			return err
		}
		genvalroot := new(common.Root)
		copy(genvalroot[:], gvroot[:])

		dom, err := fork.GetDomain(domtype, *genvalroot, fork.Epoch)
		sr = common.ComputeSigningRoot(htr, dom)
		if err != nil {
			return err
		}
		*signingroot, err = sr.MarshalText()
		if bodymap["signingRoot"] != nil && bodymap["signingRoot"].(string) != string(*signingroot) {
			err = errors.New("Incorrect signing root")
		}
	} else {
		err = errors.New("Type not supported")
	}

	return err
}

func publicKeysHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodGet {
		com := os.Args[1]

		c := &serial.Config{Name: com, Baud: 115200}
		s, err := serial.OpenPort(c)
		if err != nil {
			log.Fatal(err)
		}

		n, err := s.Write([]byte("shell echo off\nprompt off\ngetkeys\n"))
		if err != nil {
			s.Close()
			log.Fatal(err)
		}
		_ = n

		scanner := bufio.NewScanner(s)

		var b bytes.Buffer

		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "\"") {
				b.Write([]byte(scanner.Text()))
			}
			if strings.Contains(scanner.Text(), "}") || strings.Contains(scanner.Text(), "stored") {
				break
			}
		}
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusOK)
		w.Write(b.Bytes())

		n, err = s.Write([]byte("shell echo on\nprompt on\n"))
		if err != nil {
			s.Close()
			log.Fatal(err)
		}
		_ = n
		s.Close()
	} else {
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusMethodNotAllowed)
		w.Write([]byte("{\"error\":\"Method not allowed\"}"))
	}
}

func signHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodPost {
		if v {
			fmt.Println("Received signing request")
		}

		bod, err := ioutil.ReadAll(r.Body)
		if err != nil {
			log.Printf("Error reading body: %v", err)
			http.Error(w, "can't read body", http.StatusBadRequest)
			return
		}

		supported := false

		var signingroot []byte
		err = getSigningRoot(bod, &supported, &signingroot)

		if supported && err == nil {

			if err == nil {
				if v {
					fmt.Println("Signing root: " + string(signingroot))
				}
				com := os.Args[1]

				c := &serial.Config{Name: com, Baud: 115200}
				s, err := serial.OpenPort(c)
				if err != nil {
					log.Fatal(err)
				}

				n, err := s.Write([]byte("shell echo off\nprompt off\n"))
				if err != nil {
					s.Close()
					log.Fatal(err)
				}

				str := "signature " + r.URL.Path[18:] + " " + string(signingroot) + "\n"

				n, err = s.Write([]byte(str))
				if err != nil {
					s.Close()
					log.Fatal(err)
				}

				scanner := bufio.NewScanner(s)

				var b bytes.Buffer
				e := true

				for scanner.Scan() {
					if strings.Contains(scanner.Text(), "0x") {
						e = false
						str = scanner.Text()
						b.Write([]byte("{\r\n\t\"signature\": \"" + scanner.Text() + "\"\r\n}"))
						break
					}
					if strings.Contains(scanner.Text(), "stored") || strings.Contains(scanner.Text(), "Incorrect") {
						break
					}
				}
				if e == false {
					if v {
						fmt.Println("Signing successful")
					}
					w.Header().Set("Content-Type", "application/json")
					w.WriteHeader(http.StatusOK)
					w.Write(b.Bytes())
				} else {
					if v {
						fmt.Println("Signing failed")
					}
					w.Header().Set("Content-Type", "application/json")
					w.WriteHeader(http.StatusNotFound)
					w.Write([]byte("{\"error\": \"Key not found: " + r.URL.Path[6:] + "\"}"))
				}

				n, err = s.Write([]byte("shell echo on\nprompt on\n"))
				if err != nil {
					s.Close()
					log.Fatal(err)
				}
				_ = n

				s.Close()
			} else {
				if v {
					fmt.Println("Signing failed")
				}
				w.Header().Set("Content-Type", "application/json")
				w.WriteHeader(http.StatusInternalServerError)
				w.Write([]byte("{\"error\":\"Failed while signing\"}"))
			}
		} else {
			if v {
				fmt.Println("Signing failed")
			}
			w.Header().Set("Content-Type", "application/json")
			w.WriteHeader(http.StatusNotImplemented)
			str := "{\"error\": \"" + err.Error() + "\"}"
			w.Write([]byte(str))
		}
	} else {
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusMethodNotAllowed)
		w.Write([]byte("{\"error\":\"Method not allowed\"}"))
	}
}

func badReqHandler(w http.ResponseWriter, r *http.Request) {
	http.Error(w, "Bad request format", http.StatusBadRequest)
	return
}

func upcheckHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodGet {
		var b bytes.Buffer
		b.Write([]byte("{\"status\": \"OK\"}"))
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusOK)
		w.Write(b.Bytes())
	} else {
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusMethodNotAllowed)
		w.Write([]byte("{\"error\":\"Method not allowed\"}"))
	}
	return
}

func main() {
	if len(os.Args) == 4 || (len(os.Args) == 5 && os.Args[4] == "-v") {
		if len(os.Args) == 5 {
			v = true
		}
		http.HandleFunc("/api/v1/eth2/sign/", signHandler)
		http.HandleFunc("/", badReqHandler)
		http.HandleFunc("/sign", badReqHandler)
		http.HandleFunc("/upcheck", upcheckHandler)
		http.HandleFunc("/api/v1/eth2/publicKeys", publicKeysHandler)

		var ksmap map[string]interface{}
		ksjson, err := os.ReadFile(os.Args[2])
		json.Unmarshal(ksjson, &ksmap)
		if ksmap["path"] != nil {
			err = decryptEIP2335(&ksjson)
		} else {
			err = decryptWeb3()
		}
		if err != nil {
			fmt.Println("Failed processing keystore")
		} else {
			com := os.Args[1]

			c := &serial.Config{Name: com, Baud: 115200}
			s, err := serial.OpenPort(c)
			if err != nil {
				log.Fatal(err)
			}

			n, err := s.Write([]byte("shell echo off\nprompt off\n"))
			if err != nil {
				s.Close()
				log.Fatal(err)
			}
			n, err = s.Write([]byte("import " + sk + "\n"))
			if err != nil {
				s.Close()
				log.Fatal(err)
			}
			_ = n

			scanner := bufio.NewScanner(s)
			e := true
			for scanner.Scan() {
				if ((strings.HasPrefix(scanner.Text(), "0x")) && (strings.Contains(scanner.Text(), pkhex))) || (strings.Contains(scanner.Text(), "already")) {
					e = false
					break
				} else if strings.HasPrefix(scanner.Text(), "Incorrect") || strings.Contains(scanner.Text(), "reached") {
					break
				}
			}
			n, err = s.Write([]byte("shell echo on\nprompt on\n"))
			if err != nil {
				s.Close()
				log.Fatal(err)
			}
			s.Close()
			if e {
				fmt.Println("Failed importing key")

			} else {
				fmt.Println("Key imported")
				fmt.Println("Starting server at port 80")
				log.Fatal(http.ListenAndServe(":80", nil))
			}
		}
	} else {
		fmt.Println("Usage: " + os.Args[0] + " <comPort> " + "<keystore_path> " + "<keystore_password> " + "[-v]")
	}
}
