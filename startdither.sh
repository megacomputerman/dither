#!/bin/bash

if [ $# -lt 1 ]; then
   echo "Ops, faltou informar o nome da imagem com a extesão!!!"
   exit 1
fi

filename=$1
filename="${filename%.*}"

fileNew=$filename
fileAux=$filename

filename+=".txt"
fileAux+="_new.txt"
fileNew+="_new.bmp" 

echo $filename
echo $fileAux
echo $fileNew

path=$(pwd)

echo $path

# convert image to txt
convert $1 $filename

# create new txt with dither
$path/dither $filename

# convert new txt to new image
convert $fileAux $fileNew

# Show the images 
shotwell $1 &
shotwell $fileNew &


