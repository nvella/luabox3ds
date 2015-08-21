#!/bin/sh

cat ftpbatch.txt | ftp anonymous@$1 5000
