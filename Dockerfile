FROM centos:8
RUN yum install cmake make gcc-c++ -y
COPY . /app
WORKDIR /app
RUN cmake .
RUN make

FROM centos:8
COPY --from=0 /app/pocker /usr/local/pocker/bin/pocker
COPY --from=0 /app/data/ /usr/local/pocker/data/
RUN mkdir -p /usr/local/pocker/data/images/busybox \
  && tar -xf /usr/local/pocker/data/tar/busybox.tar -C /usr/local/pocker/data/images/busybox
ENV PATH="/usr/local/pocker/bin:${PATH}"