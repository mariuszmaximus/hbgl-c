CC = $(shell which gcc || which clang || which cc)
AR = ar
ARFLAGS = rcs
LIBNAME = libhbgl.a

ifeq ($(OS),Windows_NT)
    DESTDIR = lib/win/mingw64
    CFLAGS = -Wall -Wextra -O3
else
    DESTDIR = lib/linux/gcc
    CFLAGS = -Wall -Wextra -O3
endif

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: directories $(DESTDIR)/$(LIBNAME)
	rm -rf $(OBJ_DIR)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DESTDIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(DESTDIR)/$(LIBNAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

clean:
	@rm -rf $(OBJ_DIR) $(DESTDIR)/$(LIBNAME)

install: $(DESTDIR)/$(LIBNAME)
    # install -d $(LIBDIR)
    # install -m 644 $(DESTDIR)/$(LIBNAME) $(LIBDIR)
    # install -d $(INCLUDEDIR)
    # install -m 644 include/*.h $(INCLUDEDIR)

.PHONY: all clean install directories
