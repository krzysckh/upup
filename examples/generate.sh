#!/bin/bash

[ -f "../upup" ] || (echo "upup not compiled in .." && exit 1)

for i in *.uu
do
	name="$(echo $i | sed 's/.uu//')"
	../upup -f "$i" -o "$name".html -t "$name" -T html -l "style.css"
done
