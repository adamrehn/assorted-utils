# When compiling for iOS, be sure to EXPORT a value for "SDKROOT".
# This will switch off all of the OSX-specific functionality

# If the AR environment variable is not set, simply set it to ar
ifeq ($(AR),)
	AR = ar
endif

# If the CC environment variable is not set, simply set it to gcc
ifeq ($(CC),)
	CC = gcc
endif

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

SRC_DIR = ./source
UNAME := $(shell uname)
CXXFLAGS += -Wall -g
CREATELIB = $(AR) rcs $(BUILD_DIR)/lib/libsimplesock.a $(OBJECTS)
MESSAGE = Running under $(UNAME) with prefix $(PREFIX)

ISMINGW = $(shell uname | grep -E -c "MINGW32")
ifeq ($(ISMINGW),0)
	ifeq ($(SDKROOT),)
		LBITS := $(shell getconf LONG_BIT)
		ifeq ($(LBITS),64)
			CXXFLAGS += -fPIC
		endif
	endif
endif

# Determine if we are compiling for OSX (and NOT iOS)
ifeq ($(UNAME), Darwin)
	ifeq ($(SDKROOT),)
		MESSAGE := $(MESSAGE), produce fat object files and static lib...
		CXXFLAGS += -Wall -arch i386 -arch x86_64
		CREATELIB = libtool -static -o ./lib/libsimplesock.a $(SRC_DIR)/*.o
	else
		
		# When compiling for iOS, the static library will be built as single-architecture and then combined with other architectures using lipo
		CREATELIB = $(AR) -r $(BUILD_DIR)/lib/libsimplesock.a $(OBJECTS)
		
	endif
endif

# Do not modify CFLAGS when compiling for iOS
ifeq ($(SDKROOT),)
	CFLAGS += $(CXXFLAGS)
endif

# Library objects
OBJECTS = $(BUILD_DIR)/obj/SimpleSock.o

all: dirs $(OBJECTS) $(OSX_COMPAT_OBJECTS)
	@echo $(MESSAGE)...
	$(CREATELIB)
	cp $(SRC_DIR)/*.h $(BUILD_DIR)/include/simplesock/

$(BUILD_DIR)/obj/SimpleSock.o: $(SRC_DIR)/SimpleSock.cpp $(SRC_DIR)/SimpleSock.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/obj || mkdir $(BUILD_DIR)/obj
	@test -d $(BUILD_DIR)/lib || mkdir $(BUILD_DIR)/lib
	@test -d $(BUILD_DIR)/include || mkdir $(BUILD_DIR)/include
	@test -d $(BUILD_DIR)/include/simplesock || mkdir $(BUILD_DIR)/include/simplesock

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/lib || mkdir $(PREFIX)/lib
	@test -d $(PREFIX)/include || mkdir $(PREFIX)/include
	@test -d $(PREFIX)/include/simplesock || mkdir $(PREFIX)/include/simplesock

install: install_dirs
	cp -r $(BUILD_DIR)/include/* $(PREFIX)/include/
	cp $(BUILD_DIR)/lib/*.* $(PREFIX)/lib/
	chmod 755 $(PREFIX)/lib/libsimplesock.a
	chmod 755 $(PREFIX)/include/simplesock
	chmod 755 $(PREFIX)/include/simplesock/*.h

clean:
	rm $(BUILD_DIR)/obj/obj/*.o
