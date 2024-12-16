# SONOS

A C++ library and a command line tool for [Nostr](https://nostr.com). It follows [NIP-01](https://github.com/nostr-protocol/nips/blob/master/01.md) spec.

## Prerequisite to build

This project uses [miniforge](https://github.com/conda-forge/miniforge) as dependency manager. Follow [instructions](https://github.com/conda-forge/miniforge#install) to install it.

## For users

### Build and Install

See [Prerequisite](#prerequisite-to-build) before build.

```
git clone --recurse-submodules https://github.com/bvcxza/sonos.git
cd sonos
mamba env update -n $(basename $PWD) -f conda.yml
. env.sh
bld_secp256k1 && cnf -DCMAKE_INSTALL_PREFIX=~/.local && bld install
```

### Run

Just enter `sonos` to show usage.

```
Commands:

convert:

	Convert bech32 addresses (npub1 and nsec1) to/from hexadecimal string.
	Usage: sonos convert <bech32 address>
	Usage: sonos convert <hex address> <hrp:npub or nsec>

event:

	Create and send a event to nostr relays.
	Usage: echo "content" | sonos event <nsec> <kind> <tags> <relay_addresses ...>

keypair:

	Create or show keypair from hexadecimal/bech32 secret string
	Usage: sonos keypair [secret]

relays:

	Show user's relays from hexadecimal/bech32 public key.
	Usage: sonos relays <read|write|all> <public_key> <relay_addresses ...>

req:

	Send a REQ message to nostr relays and execute a external command for each received event.
	The program called by command receives the event JSON file path as the first parameter ($1 in bash).
	Usage: sonos req <filters> <command> <relay_addresses ...>

user:

	Show user metadata from hexadecimal/bech32 public key.
	Usage: sonos user <public_key> <relay_addresses ...>

```

### Send a REQ and handle events with external program

Example: Call a [bash script](samples/script.sh) for each [event](https://github.com/nostr-protocol/nips/blob/master/01.md#events-and-signatures) with tag "Monero".

```
sonos req '{"kinds":[1],"#t":["Monero"],"limit":0}' ./samples/script.sh nostr.bitcoiner.social:443
```

## For developers

### Build and Test

See [Prerequisite](#prerequisite-to-build) before build.

```
git clone https://github.com/bvcxza/sonos.git
cd sonos
git submodule update --init --force --recursive
mamba env update -n $(basename $PWD) -f conda.yml
. env.sh
bld_secp256k1 && cnf && bld && tst
```

### Run

Just enter `run` to show usage.

## Donations

Please consider donating to support the development of this project.

[Monero (XMR)](https://www.getmonero.org): 8AADjm5nz4GXXn7Tf6FNfwCaAjAdkvdUs5KgRwBGUj2NHwWqkxbfLzYPom3mL6a1cN1aypyfvyzaxHAM8aARbafFKkABT6Z

