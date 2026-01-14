#!/bin/sh
#first bash script
#Auther: Ahmed Elgabry


set -e
set -u

if [ $# -lt 2 ]
then
	echo "missing arguments"
	exit 1
fi

if [ ! -d $1 ] 
then
	echo "path is wrong"
	exit 1
fi

files=$(grep -rl "$2" "$1" | wc -l )
matches=$(grep -r "$2" "$1" | wc -l)

echo "The number of files are ${files} and the number of matching lines are ${matches}"

exit 0


