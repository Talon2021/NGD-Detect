#!/bin/bash

# 创建 algo_detect 文件夹
if [ ! -d "algo_detect_NGD" ];  then
    mkdir -p algo_detect_NGD/bin algo_detect_NGD/include algo_detect_NGD/lib algo_detect_NGD/model
fi

cp -r middleware/inc/* algo_detect_NGD/include/

cp -r my_gui/my_test algo_detect_NGD/bin/

cp -rf middleware/lib/lib* algo_detect_NGD/lib/
cp -r my_gui/my_test.cpp algo_detect_NGD/


cp -r README.md algo_detect_NGD/

latest_folder=$(ls -dt /home/share/model/*/ | head -n 1)

cp -r "${latest_folder}"* algo_detect_NGD/model/