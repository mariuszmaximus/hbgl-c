CC = $(shell which gcc || which clang || which cc)
CFLAGS = -Wall -Wextra -O3
INCLUDES = -I../include
EXAMPLES = $(wildcard *.c)

ifeq ($(OS),Windows_NT)
    EXECUTABLES = $(EXAMPLES:.c=.exe)
    LIBS = -L../lib/win/mingw64 -lhbgl -lglfw3 -lopengl32 -lm
else
    EXECUTABLES = $(EXAMPLES:.c=)
    LIBS = -L../lib/linux/gcc -lhbgl -lGL -lglfw -lm
endif

all: $(EXECUTABLES)

%.exe: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

clean:
ifeq ($(OS),Windows_NT)
	rm -f *.exe
else
	rm -f $(EXECUTABLES)
endif

run: $(EXECUTABLES)
	@for exe in $(EXECUTABLES); do \
		echo "Run $$exe..."; \
		./$$exe; \
	done
