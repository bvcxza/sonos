#!/bin/bash
# Proof of Concept Bot

# bot private key (nsec)
bot_nsec="nsec1...changethis"
# bot write relays
bot_w_relays="nostr.bitcoiner.social:443 nos.lol:443"

bot_npub=$(sonos keypair $bot_nsec | head -n1)
bot_pubkey=$(sonos convert $bot_npub npub)
script="/tmp/${bot_pubkey}_script.sh"
relays_stage="/tmp/${bot_pubkey}_relays_stage"

cat > $script << EOF0
event_id=\$(jq -r '.id' \$1)
pubkey=\$(jq -r '.pubkey' \$1)
npub=\$(sonos convert \$pubkey npub)
npub_recipient=\$(jq -r '.content' \$1 | grep npub1 | sed -n -e 's/\(^\|\(.*\s\)\)tip\snostr:\(.\+\)\(\(\s.*\)\|$\)/\3/pi')
printf '%s\n' $bot_w_relays > $relays_stage
sonos relays read \$npub $bot_w_relays >> $relays_stage
sonos relays read \$npub_recipient $bot_w_relays >> $relays_stage
relays=\$(sort $relays_stage | uniq | xargs echo)
pubkey_recipient=\$(sonos convert \$npub_recipient)
tags="[\"e\",\"\$event_id\",\"\",\"reply\"],[\"p\",\"\$pubkey\",\"\",\"mention\"],[\"p\",\"\$pubkey_recipient\",\"\",\"mention\"]"
sonos event $bot_nsec 1 \$tags \$relays << EOF
Hi nostr:\$npub !!
I still can not tip nostr:\$npub_recipient
but maybe soon.
EOF
EOF0

sonos req "{\"kinds\":[1],\"#p\":[\"$bot_pubkey\"],\"limit\":0}" "bash $script" $bot_w_relays
