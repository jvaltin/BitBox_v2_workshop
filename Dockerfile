FROM debian:latest

ENV USER=dockeruser USER_ID=1000 USER_GID=1000

RUN groupadd --gid "${USER_GID}" "${USER}" && \
    useradd \
      --uid ${USER_ID} \
      --gid ${USER_GID} \
      --create-home \
      --shell /bin/bash \
	${USER}

RUN apt-get update
RUN apt-get install -y --no-install-recommends \
  apt-utils \
  ca-certificates \
  wget \
  build-essential \
  gcc \
  g++ \
  make \
  libncurses5 \
  python \
  python-setuptools \
  python-wheel \
  python-pip \
  python-dev \
  xz-utils \
  git \
  openssl \
  libssl-dev \
  libusb-1.0-0-dev \
  libudev-dev \
  autoconf \
  automake \
  libtool \
  procps \
  && \
  pip install hidapi && \
  pip install pyaes && \
  rm -rf /var/lib/apt

RUN mkdir /root/Downloads

COPY JLink_Linux_V634c_x86_64.deb /root/Downloads/
RUN cd /root/Downloads && dpkg -i JLink_Linux_V634c_x86_64.deb

COPY cmake-3.11.2.tar.gz /root/Downloads/
RUN cd /root/Downloads && tar -xzf cmake-3.11.2.tar.gz 
RUN cd /root/Downloads/cmake-3.11.2 && ./bootstrap && make && make install

COPY gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 /root/Downloads/
RUN cd /root/Downloads && tar -xf gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2  --strip 1 -C /usr/local/

RUN rm -rf /root/Downloads/*

RUN echo "root:dockerpassword" | chpasswd

COPY user-mapping.sh /
RUN  chmod u+x user-mapping.sh

ENTRYPOINT ["/user-mapping.sh"]
