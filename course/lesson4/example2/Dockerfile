# ============================================
# DOCKERFILE - Build và chạy Dashboard System
# ============================================

FROM ubuntu:20.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-log-dev \
    libboost-filesystem-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Build vsomeip
WORKDIR /opt
RUN git clone https://github.com/COVESA/vsomeip.git && \
    cd vsomeip && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local \
             -DENABLE_SIGNAL_HANDLING=1 && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Copy source
WORKDIR /app
COPY course/ /app/course/

# Build Dashboard Service
WORKDIR /app/course/lesson2/example2
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Configuration
ENV VSOMEIP_CONFIGURATION=/app/course/lesson2/example2/vsomeip.json

# Expose SOME/IP ports
EXPOSE 30490/udp
EXPOSE 30509/udp
EXPOSE 30510/tcp

# Default command
WORKDIR /app/course/lesson2/example2/build
CMD ["./dashboard_service", "../database.json"]
