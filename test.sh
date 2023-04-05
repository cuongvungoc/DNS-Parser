#!/bin/bash
# echo "Enter password"
# read pass
if [ $1 = "password" ]
then
  echo "The password is correct."
else
  echo "The password is incorrect, try again."
fi