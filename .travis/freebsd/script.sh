#!/bin/bash -ex

export VERSION="11.1-RELEASE"
wget -nv "https://download.freebsd.org/ftp/releases/VM-IMAGES/${VERSION}/amd64/Latest/FreeBSD-${VERSION}-amd64.qcow2.xz"
tar xJvf "FreeBSD-${VERSION}-amd64.qcow2.xz"
#docker run -v $(pwd):/openswe1r ubuntu:16.04 /bin/bash -ex /openswe1r/.travis/ubuntu/docker.sh
qemu-system-x86_64 -daemonize -display none "FreeBSD-${VERSION}-amd64.qcow2" -m 1024 -smp 4 -net user,hostfwd=tcp::10022-:22 -net nic
