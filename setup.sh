#!/bin/bash
set -e

echo "Updating package list..."
sudo apt update

echo "Installing required packages: g++, make, libsdl2-dev, git..."
sudo apt install -y build-essential libsdl2-dev git

echo "Setup Done!"