# --- STAGE 1: The Builder ---
# Use Ubuntu 22.04 as the base for building. It has stable, compatible package versions.
FROM ubuntu:22.04 as builder

# Stop standard Unix tools from asking interactive questions during install
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install the C++ compiler (g++) and standard tools
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    curl \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 2. Copy source files into the container
COPY bayesian_api.cpp .
COPY crow_all.h .

# 3. Compile inside the container
# Notice we DON'T need -I/opt/homebrew because on Linux, Boost installs to a standard system path.
RUN g++ -std=c++17 bayesian_api.cpp -o bayesian_api -lpthread -DCROW_USE_BOOST -static-libstdc++ -static-libgcc

# --- STAGE 2: The Runner ---
# Use the same Ubuntu version for runtime to ensure shared library compatibility
FROM ubuntu:22.04

WORKDIR /app

# Install only the runtime Boost libraries (much smaller than the full dev tools)
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    libboost-system-dev \
    libboost-thread-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy the binary from the builder stage
COPY --from=builder /app/bayesian_api .

EXPOSE 8080

CMD ["./bayesian_api"]