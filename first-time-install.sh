#!/bin/bash

set -xeuf -o pipefail

# Install system deps
sudo apt-get update
sudo apt-get install -y fritzing fritzing-data fritzing-parts
