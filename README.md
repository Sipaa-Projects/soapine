# Soapine

### What is Soapine?

Soapine is a fork of the Limine bootloader. (TODO: expand this section)

### Supported architectures
* IA-32 (32-bit x86)
* x86-64
* aarch64 (arm64)
* riscv64

### Supported boot protocols
* Linux
* Limine (the protocol specification is in the original repo)
* Multiboot 1
* Multiboot 2
* Chainloading

### Supported partitioning schemes
* MBR
* GPT
* Unpartitioned media

### Supported filesystems
* FAT12/16/32
* ISO9660 (CDs/DVDs)

### Minimum system requirements
For 32-bit x86 systems, support is only ensured starting with those with
Pentium Pro (i686) class CPUs.

All x86-64, aarch64, and riscv64 (UEFI) systems are supported.

## Binary releases

For convenience, for point releases, binaries are distributed. These binaries
are shipped in the `-binary` branches and tags of this repository
(see [branches](https://github.com/Sipaa-Projects/Soapine/branches/all) and
[tags](https://github.com/Sipaa-Projects/Soapine/tags)).

For example, to clone the latest binary release of the `7.x` branch, one can do:
```bash
git clone https://github.com/Sipaa-Projects/Soapine.git --branch=v7.x-binary --depth=1
```
or, to clone a specific binary point release (for example `7.9.1`):
```bash
git clone https://github.com/Sipaa-Projects/Soapine.git --branch=v7.9.1-binary --depth=1
```

In order to rebuild host utilities like `Soapine`, simply run `make` in the binary
release directory.

Host utility binaries are provided for Windows.

## Building the bootloader

*The following steps are not necessary if cloning a binary release. If so, skip to*
*"Installing Soapine binaries".*

### Building with Nix

This repository provides [Nix](https://nixos.org/)-based tooling for a convenient
development environment and building Soapine using Nix.

To use the regular build flow using a toolchain obtained by Nix, simply
run `$ nix develop` to open a Nix shell. Then follow the guide below,
skipping the "Prerequisites" step.

### Regular build

#### Prerequisites

In order to build Soapine, the following programs have to be installed:
common UNIX tools (also known as `coreutils`),
`GNU make`, `grep`, `sed`, `find`, `awk`, `gzip`, `nasm`, `mtools`
(optional, necessary to build `limine-uefi-cd.bin`).
Furthermore, `gcc` or `llvm/clang` must also be installed, alongside
the respective binutils.

#### Configure

If using a release tarball (recommended, see https://github.com/Sipaa-Projects/Soapine/releases),
run `./configure` directly.

If checking out from the repository, run `./bootstrap` first in order to download the
necessary dependencies and generate the configure script (`GNU autoconf` required).

`./configure` takes arguments and environment variables; for more information on
these, run `./configure --help`.

**`./configure` by default does not build any Soapine port. Make sure to read the**
**output of `./configure --help` and enable any or all ports!**

Soapine supports both in-tree and out-of-tree builds. Simply run the `configure`
script from the directory you wish to execute the build in. The following `make`
commands are supposed to be run inside the build directory.

#### Building Soapine

To build Soapine, run:
```bash
make    # (or gmake where applicable)
```

The generated bootloader files are going to be in `./bin`.

## Installing Soapine binaries

This step is optional as the bootloader binaries can be used from the `./bin` or
release directory just fine. This step will only install them to `share`, `include`, and
`bin` directories in the specified prefix (default is `/usr/local`, see
`./configure --help`, or the `PREFIX` variable if installing from a binary release).

To install Soapine, run:
```bash
make install    # (or gmake where applicable)
```

## Usage

See [USAGE.md](USAGE.md).

## Acknowledgments
Soapine uses a stripped-down version of [tinf](https://github.com/jibsen/tinf) for early GZIP decompression.

Soapine relies on [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) for runtime GZIP decompression and image loading.

Soapine uses a patched version of libfdt (can be found in Linux's source tree) for manipulating FDTs.
