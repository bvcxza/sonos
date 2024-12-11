#!/bin/bash
# $1 - path to event JSON file
event_file_path=$1
# $2 - origin relay address
# add one more relay
relay_addresses="$2 nos.lol:443"

# Get user metdada
echo "> User:"
sonos user $(jq -r '.pubkey' $event_file_path) $relay_addresses | jq '.'
# Format event JSON file
echo "> Message:"
jq '.' $event_file_path
