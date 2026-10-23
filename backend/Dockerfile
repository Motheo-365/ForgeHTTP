FROM ubuntu:24.04 AS builder

RUN apt-get update && \
    apt-get install -y \
    g++ \
    cmake \
    make \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build && \
    cmake --build build && \
    ctest --test-dir build --output-on-failure


FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y libsqlite3-0 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/forge /app/forge

ENV PORT=8086
ENV FORGEHTTP_DB_PATH=/data/forgehttp.db
VOLUME ["/data"]

EXPOSE 8086

CMD ["./forge"]