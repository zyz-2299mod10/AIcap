#!/bin/bash
obj_name="double_yellow_lines"

a=1
for i in ./data/${obj_name}/*.png; do
    new=$(printf "%03d.png" "$a")  # 04 pad to length of 4
    mv -i -- "$i" "./data/${obj_name}/$new"
    let a=a+1
done

mv -r "./data/${obj_name}" "./data/${obj_name}_screenshot"