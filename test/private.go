package main
import "fmt"
import ks "github.com/prysmaticlabs/prysm/shared/keystore"
import "encoding/hex"
import "encoding/json"
import "github.com/protolambda/go-keystorev4"
import "os"
func main() {
    fmt.Println("hello world")

	var sk string
	var pkhex string

	ks := new(ks.Keystore)
	keys, err := ks.GetKey("./web3/keystore-m_12381_3600_0_0_0-1642162977.json", "123456789")
	if err == nil {
		sk = hex.EncodeToString(keys.SecretKey.Marshal())
		fmt.Println(sk)
		pkhex = "0x" + hex.EncodeToString(keys.PublicKey.Marshal())
		fmt.Println(pkhex)
	}

	ksjson, err := os.ReadFile("./eip2335/keystore-m_12381_3600_0_0_0-1642162977.json")
	var ks2 keystorev4.Keystore
	err = json.Unmarshal(ksjson, &ks2)
	var pass string
	pass = "123456789"
	skbin, err := ks2.Decrypt([]byte(pass))
	if err == nil {
		sk = hex.EncodeToString(skbin)
		pkhex = "0x" + hex.EncodeToString(ks2.Pubkey)
	}
}