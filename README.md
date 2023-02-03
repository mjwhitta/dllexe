# What is this?

This is a simple C++ template to create a DLL or EXE running the same
code. It should work out-of-the-box.

You can modify `payload.cpp` but you shouldn't need to modify
`main.cpp` at all. If you prefer vscode to a terminal, there are
included launch configurations as well. Those may need updated to fit
your needs.

## Dependencies

### Arch

```
$ sudo pacman -S make mingw-w64-gcc mingw-w64-headers
```

### Debian

```
$ sudo apt install g++-mingw-w64 make
```

### Windows

```
$ choco install make mingw
```

## Example

```
$ make setup BIN=local/calc.min
$ make
$ # OR
$ make setup BIN=local/calc.min KEY=redteam
$ make ALLOC=HEAP EXEC=ENUMCALENDARINFO
```

## Make targets

The default target is `all` which will make the DLL and EXE.

Target     | Description
---        | ---
clean      | Remove the `build` directory
dll        | Compile the DLL
exe        | Compile the EXE
rundll     | Compile and run DLL with `rundll32` (Windows only)
runexe     | Compile and run EXE (Non-Windows attempts to use wine)
setup      | Create `payload.h` from `local/payload.bin`
superclean | Remove the `build` directory and `payload.h`

You can extend `make` functionality by adding targets to
`local/Makefile` which will be ignored by git along with `payload.h`
and a handful of other things in `.gitignore`.

## Make options

### ALLOC

Method for allocation. Supported values are:

- `HEAP`
- `STACK`

Default is `STACK`.

### BIN

Path to bin file containing payload. Default is `local/payload.bin`.

### EXEC

Method for executing shellcode. Supported values are:

- `CERTENUMSYSTEMSTORE`
- `COPYFILE2`
- `CRYPTENUMOIDINFO`
- `ENUMCALENDARINFO`
- `ENUMDISPLAYMONITORS`
- `ENUMPWRSCHEMES`
- `ENUMPROPS`
- `ENUMWINDOWS`
- `SETTIMER`
- `THREAD`

Default is `THREAD` (meaning no shenanigans, just `CreateThread()`).

### KEY

Key to xor the shellcode with. Default is `none` (meaning no xor).
