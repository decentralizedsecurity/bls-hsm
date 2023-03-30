#!/usr/bin/bash
usage(){
  echo "Usage: $0 [-p <project>]
  -p: <project>: project to flash [cli,remote_signer]"
  exit 1;
}

while getopts ":p:" opt; do
  case $opt in
    p) proj="$OPTARG"
  esac
done

if [ -z $proj ]; then
	usage
fi

if [ $proj != "cli" ] && [ $proj != "remote_signer" ]; then
  echo "Invalid project. Choose 'cli' or 'remote_signer'"
  exit 1;
fi

bls=`pwd`
source ~/ncs/zephyr/zephyr-env.sh
cd $bls/$proj
west flash --erase