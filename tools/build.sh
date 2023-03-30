
usage(){
  echo "Usage: $0 [-c <compiler path>] [-b <board identifier>] [-p <project>]
  -c <compiler path>: define path of the arm compiler (arm-none-eabi-gcc file)
  -b: <board identifier>
  -p: <project>: project to build [cli,remote_signer]"
  exit 1;
}

bls=`pwd`

while getopts ":c::b::p:" opt; do
  case $opt in
    c) comp="$OPTARG"
    ;;
    b) board="$OPTARG"
    ;;
    p) proj="$OPTARG"
  esac
done

if [ -z $comp ] || [ -z $board ] || [ -z $proj ]; then
	usage
fi

if [ $proj != "cli" ] && [ $proj != "remote_signer" ]; then
  echo "Invalid project. Choose 'cli' or 'remote_signer'"
  exit 1;
fi

echo "export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb" > ~/.zephyrrc
echo "export GNUARMEMB_TOOLCHAIN_PATH=${comp%bin*}" >> ~/.zephyrrc

source ~/ncs/zephyr/zephyr-env.sh

cd $bls/$proj
west build -p -b $board