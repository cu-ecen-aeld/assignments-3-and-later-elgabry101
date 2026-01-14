#!/bin/sh
#first bash script
#Auther: Ahmed Elgabry


set -e
set -u

if [ $# -ne 2 ]
then
	echo "insert all arguments"
	exit 1
fi

path=$1
dir=$(dirname "$path")
mkdir -p $dir
echo $2 > $1

exit 0
