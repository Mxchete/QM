# debian-based image for building
FROM debian:12

# essential packages for image
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        clang-format && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# directory for project
WORKDIR /workspace

CMD ["/bin/bash"]
