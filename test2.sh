#!/bin/sh

a=10
b=20

echo $a
echo $b

if [ $a -eq $b ]
then
   echo "a is equal to b"
else
   echo "a is not equal to b"
fi