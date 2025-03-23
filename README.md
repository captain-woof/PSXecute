# Setup

## Prepare Mips32 VM

```
cm VM
mkdir build
cd build
cmake ..
make
```

This creates the VM at `VM/build/PSXecute.exe`.

## Installing LLVM

```
sudo apt install llvm llvm-dev clang libclang-dev
```

## Installing MIPS toolchain

### Getting crosstool-ng

```
tar -xvf crosstool-ng-1.27.0.tar.xz
cd crosstool-ng-1.27.0

sudo apt install texinfo help2man libtool-bin bison # missing dependencies

./configure --prefix=/lib/crosstool-ng
make
make install

cd ~/.local/bin
ln -s /lib/crosstool-ng/bin/ct-ng ct-ng
```

### `mipsel-unknown-linux-gnu` toolchain

```
cd Payload
mkdir toolchain
cd toolchain

ct-ng menuconfig
```

Then make following changes:

- Target Options
    - Target Architecture > mips
    - Endianness -> Little Endian
    - Bitness -> 32 bit
    - Floating point -> software
- Operating System
    - Target OS -> linux
- Paths and Misc Options
    - Local tarballs directory -> /tmp/crosstool-tmp-tarballs
    - Prefix directory -> .
    - Working directory -> .build

Then:

```
mkdir -p /tmp/crosstool-tmp-tarballs
ct-ng build
export PATH=${PATH}:Payload/toolchain/bin
```

## Compiling transpilers

Do:

```
cd Transpiler
./compile.sh
```

# Compiling payloads

Modify `Payload/src/main.c`, and then compile:

```
cd Payload
./compile.sh
```

# Running payloads

```
PSXecute.exe payload.bin
```