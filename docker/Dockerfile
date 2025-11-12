# 使用scratch作为基础镜像
FROM scratch
# 添加Ubuntu rootfs
ADD ubuntu-base-22.04-base-amd64.tar.gz /
# 设置工作目录
WORKDIR /
# 安装必要的软件包
RUN apt-get update && apt-get install -y \
	bash coreutils \
	curl \
	wget \
	vim \
	iputils-ping \
	openssl \
	build-essential \
	make \
	gcc \
	g++ \
	git \
	net-tools \
	ssh \
	&& apt-get clean && rm -rf /var/lib/apt/lists/*
# 设置默认命令
CMD ["/bin/bash"]
