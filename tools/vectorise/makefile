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

# Under OS X, we use clang++ as the compiler and ensure we link against libc++
ifeq ($(UNAME), Darwin)
	CXX = clang++
	LDFLAGS += -lc++
endif

# Determine if we are building the OpenMP version
# (By default, we don't use OpenMP under OS X unless explicitly specified, since
#  OpenMP support was only officially added to mainline Clang in LLVM 3.6)
USE_OPENMP = 1
ifeq ($(UNAME), Darwin)
	ifeq ($(ENABLE_CLANG_OPENMP),)
		USE_OPENMP = 0
	endif
endif

# Set our targets based on whether or not we are building with OpenMP
ifeq ($(USE_OPENMP),1)
	OMP_MESSAGE = OpenMP build enabled.
	TARGETS = $(BUILD_DIR)/bin/vectorise$(EXE_EXT) $(BUILD_DIR)/bin/vectorise-omp$(EXE_EXT)
	CHMOD_CMD = chmod 777 $(PREFIX)/bin/vectorise$(EXE_EXT) && chmod 777 $(PREFIX)/bin/vectorise-omp$(EXE_EXT)
else
	OMP_MESSAGE = 'OpenMP build disabled. To enable, use `make [install] ENABLE_CLANG_OPENMP=1`.'
	TARGETS = $(BUILD_DIR)/bin/vectorise$(EXE_EXT)
	CHMOD_CMD = chmod 777 $(PREFIX)/bin/vectorise$(EXE_EXT)
endif

# Set the compiler flag to enable OpenMP if we are building with it
ifeq ($(USE_OPENMP),1)
	ifeq ($(UNAME), Darwin)
		OPENMP_FLAGS = -Xclang -fopenmp=liomp5
	else
		OPENMP_FLAGS = -fopenmp
	endif
endif

all: message dirs $(TARGETS)
	@echo Done!

$(BUILD_DIR)/bin/vectorise$(EXE_EXT): ./vectorise.cpp
	$(CXX) -o $@ ./vectorise.cpp $(CXXFLAGS) $(LDFLAGS)

$(BUILD_DIR)/bin/vectorise-omp$(EXE_EXT): ./vectorise.cpp
	$(CXX) -o $@ ./vectorise.cpp -D_ENABLE_OPENMP $(CXXFLAGS) $(LDFLAGS) $(OPENMP_FLAGS)

message:
	@echo $(OMP_MESSAGE)

dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/bin || mkdir $(BUILD_DIR)/bin

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/bin || mkdir $(PREFIX)/bin

install: message install_dirs
	cp -r $(BUILD_DIR)/bin/* $(PREFIX)/bin/
	$(CHMOD_CMD)

clean:
	rm $(BUILD_DIR)/bin/*
