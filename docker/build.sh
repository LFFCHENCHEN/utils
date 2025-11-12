!#/bin/bash
set -e
# 检查是否已下载rootfs文件
rootfs_version="22.04"
rootfs_filename="ubuntu-base-${rootfs_version}-base-amd64.tar.gz"

if [ ! -f "${rootfs_filename}" ]; then
	echo "Downloading Ubuntu ${rootfs_version} rootfs..."
	wget "http://cdimage.ubuntu.com/ubuntu-base/releases/${rootfs_version}/release/${rootfs_filename}"
else
	echo "Ubuntu ${rootfs_version} rootfs already exists."
fi

# 检查是否已创建Dockerfile
if [ ! -f "Dockerfile" ]; then
	echo "DOCKERFILE not found!"
	exit 1
fi

# 构建镜像
docker build -t my-ubuntu:22.04 .
