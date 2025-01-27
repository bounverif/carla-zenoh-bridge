# libcarla build stage
FROM ubuntu:20.04 AS builder
WORKDIR /home


# Following run commands are organized in accordance with carla 0.9.15 build guide.

# Install software requirements.
RUN <<EOF
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata
apt-get install -y wget software-properties-common &&
add-apt-repository -y ppa:ubuntu-toolchain-r/test &&
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add
EOF

# Install Ubuntu 20.04 requirements.
# Because the build process avoids building Unreal Engine, we also need to install 
# C++ stdlib 10.0
RUN <<EOF
apt-add-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal main"
apt-get update
apt-get install -y build-essential clang-10 lld-10 g++-7 cmake ninja-build libvulkan1 \
		python python-dev python3-dev python3-pip libpng-dev libtiff5-dev \
		libjpeg-dev libc++-10-dev libc++abi-10-dev sed curl unzip autoconf \
		libtool rsync libxml2-dev git
update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-10/bin/clang++ 180 &&
update-alternatives --install /usr/bin/clang clang /usr/lib/llvm-10/bin/clang 180
EOF

# Clone carla repository.
# Carla assets link has been changed, so edit the link in the Update.sh file accordingly.
# Uncomment the line below to install latest assets.
RUN <<EOF
apt-get install -y aria2
git clone --depth=1 --branch=0.9.15 https://github.com/carla-simulator/carla
cd carla
sed -i 's@http://carla-assets.s3.amazonaws.com/\${CONTENT_ID}.tar.gz@https://carla-assets.s3.us-east-005.backblazeb2.com/\${CONTENT_ID}.tar.gz@' Update.sh
# ./Update.sh
EOF

WORKDIR /home/carla

# Modify build scripts to enable installation without Unreal Engine 4.26.
# First 6 lines modify script variables to point to custom directories on the system instead of those defined
# by Unreal Engine environment.
# Next 2 lines fixes errors present in the build scripts.
RUN <<EOF
sed -i 's@export CC="\$UE4_ROOT/Engine/Extras/ThirdPartyNotUE/SDKs/HostLinux/Linux_x64/v17_clang-10.0.1-centos7/x86_64-unknown-linux-gnu/bin/clang"@export CC="/usr/bin/clang-10"@' Util/BuildTools/Setup.sh
sed -i 's@export CXX="\$UE4_ROOT/Engine/Extras/ThirdPartyNotUE/SDKs/HostLinux/Linux_x64/v17_clang-10.0.1-centos7/x86_64-unknown-linux-gnu/bin/clang++"@export CXX="/usr/bin/clang++-10"@' Util/BuildTools/Setup.sh
sed -i 's@export PATH="\$UE4_ROOT/Engine/Extras/ThirdPartyNotUE/SDKs/HostLinux/Linux_x64/v17_clang-10.0.1-centos7/x86_64-unknown-linux-gnu/bin\:\$PATH"@\n@' Util/BuildTools/Setup.sh
sed -i 's@LLVM_INCLUDE="\$UE4_ROOT/Engine/Source/ThirdParty/Linux/LibCxx/include/c++/v1"@LLVM_INCLUDE="/usr/lib/llvm-10/include/c++/v1"@' Util/BuildTools/Setup.sh
sed -i 's@LLVM_LIBPATH="\$UE4_ROOT/Engine/Source/ThirdParty/Linux/LibCxx/lib/Linux/x86_64-unknown-linux-gnu"@LLVM_LIBPATH="/usr/lib/llvm-10/lib"@' Util/BuildTools/Setup.sh
sed -i 's@UNREAL_HOSTED_CFLAGS="--sysroot=\$UE4_ROOT/Engine/Extras/ThirdPartyNotUE/SDKs/HostLinux/Linux_x64/v17_clang-10.0.1-centos7/x86_64-unknown-linux-gnu/"@UNREAL_HOSTED_CFLAGS="--sysroot=/"@' Util/BuildTools/Setup.sh
sed -i 's@setup downloadplugins:@setup: downloadplugins@' Util/BuildTools/Linux.mk
sed -i 's@https://boostorg.jfrog.io/artifactory/main/release/\${BOOST_VERSION}/source/\${BOOST_PACKAGE_BASENAME}.tar.gz@https://archives.boost.io/release/\${BOOST_VERSION}/source/\${BOOST_PACKAGE_BASENAME}.tar.gz@' Util/BuildTools/Setup.sh
EOF

# Build libcarla
RUN <<EOF
cd Examples/CppClient
make
EOF

FROM ubuntu:20.04 AS bridge
WORKDIR /home
RUN <<EOF
apt-get update
apt-get -y install git
EOF
RUN git clone https://github.com/bounverif/carla-zenoh-bridge.git
COPY --from=builder /home/carla/Examples/CppClient/libcarla-install ./carla-zenoh-bridge/

