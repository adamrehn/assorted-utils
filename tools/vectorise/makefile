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

# Under OS X, we use clang++ as the compiler and ensure we link against libstdc++
ifeq ($(UNAME), Darwin)
	CXX = clang++
	LDFLAGS += -lstdc++
endif

all: dirs $(BUILD_DIR)/bin/vectorise$(EXE_EXT) $(BUILD_DIR)/bin/vectorise-omp$(EXE_EXT)
	@echo Done!

$(BUILD_DIR)/bin/vectorise$(EXE_EXT): ./vectorise.cpp
	$(CXX) -o $@ ./vectorise.cpp $(CXXFLAGS) $(LDFLAGS)


ifeq ($(UNAME), Darwin)

# Don't build the OpenMP version under Darwin
$(BUILD_DIR)/bin/vectorise-omp$(EXE_EXT): ./vectorise.cpp
	@echo Skipping OpenMP build under Darwin until OpenMP support is added to Clang.

else

# Under all other operating systems, build the OpenMP version
$(BUILD_DIR)/bin/vectorise-omp$(EXE_EXT): ./vectorise.cpp
	$(CXX) -o $@ ./vectorise.cpp -fopenmp -D_ENABLE_OPENMP $(CXXFLAGS) $(LDFLAGS)

endif


dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/bin || mkdir $(BUILD_DIR)/bin

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/bin || mkdir $(PREFIX)/bin

install: install_dirs
	cp -r $(BUILD_DIR)/bin/* $(PREFIX)/bin/
	chmod 777 $(PREFIX)/bin/vectorise$(EXE_EXT)
ifneq ($(UNAME), Darwin)
	chmod 777 $(PREFIX)/bin/vectorise-omp$(EXE_EXT)
endif

clean:
	rm $(BUILD_DIR)/bin/*
