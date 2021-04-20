FROM centos:8
RUN yum install cmake make gcc-c++ -y
COPY . /app
WORKDIR /app
RUN cmake .
RUN make

FROM centos:8
COPY --from=0 /app/pocker /usr/local/pocker/bin/pocker
ENV PATH="/usr/local/pocker/bin:${PATH}"