#!/bin/bash
# $1 - path to event JSON file
# Format event JSON file
jq '.' $1
