# ============================================================================
# Dockerfile - Build và chạy AUTOSAR Dashboard trong container
# ============================================================================

FROM ubuntu:22.04 AS builder

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    qtbase5-dev \
    qttools5-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Build vsomeip
WORKDIR /opt
RUN git clone https://github.com/COVESA/vsomeip.git && \
    cd vsomeip && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Copy project
WORKDIR /app
COPY . .

# Build project
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# ============================================================================
# Runtime image
# ============================================================================
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    libboost-log1.74.0 \
    libqt5widgets5 \
    libqt5gui5 \
    libqt5core5a \
    && rm -rf /var/lib/apt/lists/*

# Copy vsomeip library
COPY --from=builder /usr/local/lib/libvsomeip* /usr/local/lib/
RUN ldconfig

# Copy built binaries
COPY --from=builder /app/build/bin /app/bin
COPY --from=builder /app/config /app/config

WORKDIR /app

# Environment
ENV VSOMEIP_CONFIGURATION=/app/config/vsomeip.json
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Default command
CMD ["./bin/dashboard_service"]
