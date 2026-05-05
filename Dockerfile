FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    cmake build-essential \
    libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri \
    libxi-dev libxinerama-dev freeglut3-dev \
    libx11-dev libxext-dev \
    xvfb x11vnc \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
