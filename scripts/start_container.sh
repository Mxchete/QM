#!/bin/bash

# Debian 12 based container for ensuring reproducability - all releases should be built using this
CONTAINER_NAME="qm_build"
IMAGE_NAME="qm_debian_12"
WORKSPACE_DIR="${PWD}"

if ! podman image exists $IMAGE_NAME; then
  echo "Building the Docker image: $IMAGE_NAME"
  podman build -t $IMAGE_NAME .
fi

podman run -it --rm \
  --name $CONTAINER_NAME \
  -v "$WORKSPACE_DIR":/workspace \
  $IMAGE_NAME
