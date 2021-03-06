FROM centos:8
RUN yum install cmake make gcc-c++ -y
# BUGS for centos:8 https://bugs.centos.org/view.php?id=18212
RUN yum install libarchive-3.3.3-1.el8.x86_64 -y
COPY . /app
WORKDIR /app
RUN cmake .
RUN make

FROM centos:8
RUN yum install e4fsprogs -y
COPY --from=0 /app/pocker /usr/local/pocker/bin/pocker
COPY --from=0 /app/data/ /usr/local/pocker/data/
RUN mkdir -p /usr/local/pocker/data/images/busybox \
  && tar -xf /usr/local/pocker/data/tar/busybox.tar -C /usr/local/pocker/data/images/busybox
ENV PATH="/usr/local/pocker/bin:${PATH}"