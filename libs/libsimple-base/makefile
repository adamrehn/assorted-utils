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
CREATELIB = $(AR) rcs $(BUILD_DIR)/lib/libsimple-base.a $(OBJECTS)
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

# Determine if we are compiling for OS X (and NOT iOS)
ifeq ($(UNAME), Darwin)
	ifeq ($(SDKROOT),)
		MESSAGE := $(MESSAGE), produce fat object files and static lib...
		CXXFLAGS += -Wall -arch i386 -arch x86_64
		CREATELIB = libtool -static -o $(BUILD_DIR)/lib/libsimple-base.a $(OBJECTS)
	else
		
		# When compiling for iOS, the static library will be built as single-architecture and then combined with other architectures using lipo
		CREATELIB = $(AR) -r $(BUILD_DIR)/lib/libsimple-base.a $(OBJECTS)
		
	endif
endif

# Do not modify CFLAGS when compiling for iOS
ifeq ($(SDKROOT),)
	CFLAGS += $(CXXFLAGS)
endif

# Library objects
OBJECTS = $(BUILD_DIR)/obj/StartupArgsParser.o $(BUILD_DIR)/obj/binary_manipulation.o $(BUILD_DIR)/obj/bitwise.o $(BUILD_DIR)/obj/checksum.o $(BUILD_DIR)/obj/endianness.o $(BUILD_DIR)/obj/environment.o $(BUILD_DIR)/obj/file_manipulation.o $(BUILD_DIR)/obj/maths.o $(BUILD_DIR)/obj/multiple_input_files.o $(BUILD_DIR)/obj/sha1.o $(BUILD_DIR)/obj/string_manipulation.o $(BUILD_DIR)/obj/time.o $(BUILD_DIR)/obj/crc32.o $(BUILD_DIR)/obj/random.o

all: dirs $(OBJECTS)
	@echo $(MESSAGE)...
	$(CREATELIB)
	$(OSX_COMPAT_CREATELIB)
	$(OSX_COMPAT_CREATELIB_BUNDLEABLE)
	cp $(SRC_DIR)/*.h $(BUILD_DIR)/include/simple-base/

$(BUILD_DIR)/obj/StartupArgsParser.o: $(SRC_DIR)/StartupArgsParser.cpp $(SRC_DIR)/StartupArgsParser.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/binary_manipulation.o: $(SRC_DIR)/binary_manipulation.cpp $(SRC_DIR)/binary_manipulation.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/bitwise.o: $(SRC_DIR)/bitwise.cpp $(SRC_DIR)/bitwise.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/checksum.o: $(SRC_DIR)/checksum.cpp $(SRC_DIR)/checksum.h $(SRC_DIR)/endianness.h $(SRC_DIR)/binary_manipulation.h $(SRC_DIR)/crc32.h $(SRC_DIR)/sha1.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/endianness.o: $(SRC_DIR)/endianness.cpp $(SRC_DIR)/endianness.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/environment.o: $(SRC_DIR)/environment.cpp $(SRC_DIR)/environment.h $(SRC_DIR)/string_manipulation.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/file_manipulation.o: $(SRC_DIR)/file_manipulation.cpp $(SRC_DIR)/file_manipulation.h $(SRC_DIR)/string_manipulation.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/maths.o: $(SRC_DIR)/maths.cpp $(SRC_DIR)/maths.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/multiple_input_files.o: $(SRC_DIR)/multiple_input_files.cpp $(SRC_DIR)/multiple_input_files.h $(SRC_DIR)/array_manipulation.h $(SRC_DIR)/environment.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/sha1.o: $(SRC_DIR)/sha1.cpp $(SRC_DIR)/sha1.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/string_manipulation.o: $(SRC_DIR)/string_manipulation.cpp $(SRC_DIR)/string_manipulation.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/time.o: $(SRC_DIR)/time.cpp $(SRC_DIR)/time.h $(SRC_DIR)/environment.h $(SRC_DIR)/string_manipulation.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/obj/crc32.o: $(SRC_DIR)/crc32.c $(SRC_DIR)/crc32.h
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/obj/random.o: $(SRC_DIR)/random.cpp $(SRC_DIR)/random.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/obj || mkdir $(BUILD_DIR)/obj
	@test -d $(BUILD_DIR)/lib || mkdir $(BUILD_DIR)/lib
	@test -d $(BUILD_DIR)/include || mkdir $(BUILD_DIR)/include
	@test -d $(BUILD_DIR)/include/simple-base || mkdir $(BUILD_DIR)/include/simple-base

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/lib || mkdir $(PREFIX)/lib
	@test -d $(PREFIX)/include || mkdir $(PREFIX)/include
	@test -d $(PREFIX)/include/simple-base || mkdir $(PREFIX)/include/simple-base

install: install_dirs
	cp -r $(BUILD_DIR)/include/* $(PREFIX)/include/
	cp $(BUILD_DIR)/lib/*.* $(PREFIX)/lib/
	chmod 755 $(PREFIX)/lib/libsimple-base.a
	chmod 755 $(PREFIX)/include/simple-base
	chmod 755 $(PREFIX)/include/simple-base/*.h

clean:
	rm $(BUILD_DIR)/obj/obj/*.o
