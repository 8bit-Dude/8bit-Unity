#!/usr/bin/env bash

pip3 install pillow

cd utils/scripts

python3 builder.py "$@"
