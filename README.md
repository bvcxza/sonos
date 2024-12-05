# SONOS

A C++ library and a command line tool for [Nostr](https://nostr.com).

## Build and Test

This project uses [miniforge](https://github.com/conda-forge/miniforge) as dependency manager. Follow [instructions](https://github.com/conda-forge/miniforge#install) to install it.

```
cd sonos
git submodule update --init --force --recursive
mamba env update -n $(basename $PWD) -f conda.yml
. env.sh
bld_secp256k1 && cnf && bld && tst
```

## Run

Just enter `run` to show usage.

## Install

```
cnf -DCMAKE_INSTALL_PREFIX=~/.local && bld install
```

## Donations

Please consider donating to support the development of this project.

[Monero (XMR)](https://www.getmonero.org): 8AADjm5nz4GXXn7Tf6FNfwCaAjAdkvdUs5KgRwBGUj2NHwWqkxbfLzYPom3mL6a1cN1aypyfvyzaxHAM8aARbafFKkABT6Z

