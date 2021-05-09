#!/bin/bash

set -xeuf -o pipefail

# Install system deps
sudo apt-get update
sudo apt install libqt5printsupport5 libqt5xml5 libqt5sql5 libqt5serialport5 libqt5sql5-sqlite
sudo apt-get install -y fritzing fritzing-data fritzing-parts
