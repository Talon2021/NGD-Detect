#!/bin/bash

# 创建 algo_detect 文件夹
mkdir -p algo_detect/bin algo_detect/include algo_detect/lib algo_detect/model

cp -r middleware/inc/* algo_detect/include/

cp -r my_gui/my_test algo_detect/bin/

cp -rf middleware/lib/lib* algo_detect/lib/
cp -r  middleware/lib/network/* algo_detect/lib/
cp -r  middleware/lib/gpiod/* algo_detect/lib/

cp -r my_gui/my_test.cpp algo_detect/


cp -r README.md algo_detect/