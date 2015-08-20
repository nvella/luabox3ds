#!/bin/sh

cd bin
cat ../ftpbatch.txt | ftp anonymous@$1 5000
