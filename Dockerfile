FROM debian:testing as zvmone

RUN apt-get update -q && apt-get install -qy --no-install-recommends \
    ca-certificates g++ cmake ninja-build \
 && rm -rf /var/lib/apt/lists/*

ADD . /src
RUN mkdir /build \
 && cmake -S /src -B /build -G Ninja -DZVMONE_TESTING=ON -DHUNTER_ROOT=/build \
 && cmake --build /build --target install \
 && ldconfig \
 && rm /build -rf \
 && adduser --disabled-password --no-create-home --gecos '' zvmone
USER zvmone
