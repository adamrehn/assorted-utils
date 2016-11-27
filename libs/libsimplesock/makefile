# We can use the BUILD_DIR environment variable to set the location of the output files
ifeq ($(BUILD_DIR),)
	BUILD_DIR = ./build
endif

# We can use the PREFIX environment variable to control the installation directory
ifeq ($(PREFIX),)
	PREFIX = /usr/local
endif

SRC_DIR = ./source

all: dirs
	cp $(SRC_DIR)/*.h $(BUILD_DIR)/include/simplesock/

dirs:
	@test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
	@test -d $(BUILD_DIR)/include || mkdir $(BUILD_DIR)/include
	@test -d $(BUILD_DIR)/include/simplesock || mkdir $(BUILD_DIR)/include/simplesock

install_dirs:
	@test -d $(PREFIX) || mkdir $(PREFIX)
	@test -d $(PREFIX)/include || mkdir $(PREFIX)/include
	@test -d $(PREFIX)/include/simplesock || mkdir $(PREFIX)/include/simplesock

install: install_dirs
	cp -r $(BUILD_DIR)/include/* $(PREFIX)/include/
	chmod 755 $(PREFIX)/include/simplesock/*.h

clean:
	rm -R $(BUILD_DIR)
