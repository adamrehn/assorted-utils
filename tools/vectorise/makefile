# Detect host environment
UNAME := $(shell uname)
ISMINGW = $(shell uname | grep -E -c "MINGW32")

# If the CXX environment variable is not set, simply set it to g++
ifeq ($(CXX),)
	CXX = g++
endif

# We can use the BUILD_DIR environment variable to set the location of the output files
ifeq ($(BUILD_DIR),)
	BUILD_DIR = ./build
endif

# We can use the PREFIX environment variable to control the installation directory
ifeq ($(PREFIX),)
	PREFIX = /usr/local
endif

# Under MinGW, we want to use GCC and statically link with the standard libraries
EXE_EXT =
LDFLAGS += -lsimple-base
ifeq ($(ISMINGW),1)
	CXX = g++
	EXE_EXT = .exe
	LDFLAGS += -static-libgcc -static-libstdc++
endif

# Under OSX, we use clang++ as the compiler and ensure we link against libstdc++
ifeq ($(UNAME), Darwin)
	CXX = clang++
	LDFLAGS += -lstdc++
endif

# Object files
OBJECT_FILES = $(BUILD_DIR)/obj/vectorise.o

all: dirs $(BUILD_DIR)/bin/vectorise$(EXE_EXT)
	@echo Done!

$(BUILD_DIR)/bin/vectorise$(EXE_EXT): $(OBJECT_FILES)
	$(CXX) -o $@ $(OBJECT_FILES) $(CXXFLAGS) $(LDFLAGS)

$(BUILD_DIR)/obj/vectorise.o: ./vectorise.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/obj || mkdir $(BUILD_DIR)/obj
	@test -d $(BUILD_DIR)/bin || mkdir $(BUILD_DIR)/bin

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/bin || mkdir $(PREFIX)/bin

install: install_dirs
	cp -r $(BUILD_DIR)/bin/* $(PREFIX)/bin/
	chmod 777 $(PREFIX)/bin/vectorise$(EXE_EXT)

clean:
	rm $(BUILD_DIR)/obj/*.o
