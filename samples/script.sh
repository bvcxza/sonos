#!/bin/bash
# Proof of Concept Bot
# $1 - path to event JSON file
event_file_path=$1
# $2 - origin relay address
relay_addresses="nostr.bitcoiner.social:443 nos.lol:443"

event_id=$(jq -r '.id' $event_file_path)
pubkey=$(jq -r '.pubkey' $event_file_path)
npub=$(sonos convert $pubkey npub)
npub_recipient=$(jq -r '.content' $event_file_path | grep npub1 | sed -n -e 's/\(^\|\(.*\s\)\)tip\snostr:\(.\+\)\(\(\s.*\)\|$\)/\3/pi')
relay_addresses="$relay_addresses $(sonos relays read $npub $relay_addresses | xargs echo) $(sonos relays read $npub_recipient $relay_addresses | xargs echo)"
pubkey_recipient=$(sonos convert $npub_recipient)
nsec_bot="nsec1...changethis"
tags="[\"e\",\"$event_id\",\"\",\"reply\"],[\"p\",\"$pubkey\",\"\",\"mention\"],[\"p\",\"$pubkey_recipient\",\"\",\"mention\"]"
sonos event $nsec_bot 1 $tags $relay_addresses << EOF
Hi nostr:$npub !!
I still can not tip nostr:$npub_recipient
but maybe soon.
EOF
