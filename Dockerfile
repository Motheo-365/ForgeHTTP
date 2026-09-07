FROM ubuntu:24.04 AS builder

RUN apt-get update && \
    apt-get install -y \
    g++ \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build && \
    cmake --build build


FROM ubuntu:24.04

WORKDIR /app

COPY --from=builder /app/build/forge /app/forge

EXPOSE 10000

CMD ["./forge"]