FROM registry.cn-hangzhou.aliyuncs.com/ubuntu/ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN \
# use mirror sources
echo "deb http://mirrors.ustc.edu.cn/ubuntu/ jammy main restricted universe multiverse\n \
deb http://mirrors.ustc.edu.cn/ubuntu/ jammy-security main restricted universe multiverse\n \
deb http://mirrors.ustc.edu.cn/ubuntu/ jammy-updates main restricted universe multiverse\n \
deb http://mirrors.ustc.edu.cn/ubuntu/ jammy-backports main restricted universe multiverse" \
> /etc/apt/sources.list \
# install packages
&& apt-get update -y && apt-get upgrade -y \
&& apt-get install -y curl repo \
&& apt-get install -y bc binfmt-support bison bsdmainutils build-essential bzip2 chrpath cmake cpio cpp-aarch64-linux-gnu \
debianutils device-tree-compiler diffstat expat expect fakeroot fdisk file flex g++ gawk gcc gcc-multilib git git-core \
g++-multilib gpgv2 iputils-ping libegl1-mesa libelf-dev libgmp-dev libgucharmap-2-90-dev liblz4-tool libmpc-dev \
libsdl1.2-dev libssl-dev live-build make ncurses-dev net-tools patchelf python2 python3 python-is-python3 python3-git \
python3-jinja2 python3-pexpect python3-pip qemu-user-static rsync socat ssh strace sudo texinfo time tree unzip vim \
wget xterm xz-utils zstd \
&& pip3 install pyelftools -i http://pypi.mirrors.ustc.edu.cn/simple/ --trusted-host pypi.mirrors.ustc.edu.cn \
# add user in docker
&& useradd -c 'rk user' -m -d /home/rk -s /bin/bash rk && sed -i -e '/\%sudo/ c \%sudo ALL=(ALL) NOPASSWD: ALL' /etc/sudoers && usermod -a -G sudo rk \
&& echo "docker image build complete"
# delete useless package and cache if need
#&& apt-get autoclean && apt-get autoremove && rm -rf /var/lib/apt/lists/*