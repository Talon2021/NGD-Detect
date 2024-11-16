#!/bin/bash
###
 # @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 # @Date: 2024-05-21 16:42:15
 # @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 # @LastEditTime: 2024-05-29 11:12:55
 # @FilePath: \panoramic_code\lib\rk_lib\test.sh
 # @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
### 

# 定义库文件列表
libs=(
    liveMedia
    groupsock
    BasicUsageEnvironment
    UsageEnvironment
    easymedia
    RKAP_3A
    third_media
    rockface
    sqlite3
    rockx
    VSC
    asound
    RKAP_ANR
    RKAP_Common
    OpenVX
    GAL
    ArchModelSw
    NNArchPerf
    drm
    rockchip_mpp
    rockchip_vpu
    rknn_runtime
    rknn_api
    v4l2
    v4lconvert
    rga
    md_share
    rkaiq
    od_share
)

# 定义源路径列表
src_paths=(
    "/home/share/RV1126_RV1109_LINUX_SDK_V2.2.5.1_20230530/buildroot/output/rockchip_rv1126_rv1109/host/arm-buildroot-linux-gnueabihf/sysroot/usr/lib"
    "/home/share/RV1126_RV1109_LINUX_SDK_V2.2.5.1_20230530/buildroot/output/rockchip_rv1126_rv1109/target/lib"
    "/home/share/RV1126_RV1109_LINUX_SDK_V2.2.5.1_20230530/buildroot/output/rockchip_rv1126_rv1109/build/rknpu-1.6.1/drivers/linux-armhf-puma-mini/usr/lib"
    "/home/share/RV1126_RV1109_LINUX_SDK_V2.2.5.1_20230530/buildroot/output/rockchip_rv1126_rv1109/build/rknpu-1.6.1/rknn/rknn_utils/librknn_utils/lib"
)

# 定义目标路径
dst_path="/home/share/panoramic_code/lib/rk_lib"

# 创建目标路径
mkdir -p "$dst_path"

# 遍历库文件列表并拷贝
for lib in "${libs[@]}"; do
    found=false
    for src_path in "${src_paths[@]}"; do
        src_file="$src_path/lib$lib.so"
        if [ -f "$src_file" ]; then
            cp "$src_file" "$dst_path"
            echo "Copied $src_file to $dst_path"
            found=true
            break
        fi
    done
    if [ "$found" = false ]; then
        echo "Library $lib not found in any of the specified paths."
    fi
done

echo "All libraries copied."
