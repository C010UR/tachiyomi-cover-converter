# The CC variable sets C compiler
CC = gcc

# The CFLAGS variable sets compile flags for gcc:
CFLAGS = -g -Wall -O0 -std=c99
#  -g        compile with debug information
#  -Wall     give verbose compiler warnings
#  -On       optimization level
#  -std=c99  use the c99 standard language definition

# The LDFLAGS variable sets flags for linker
DFLAGS = 

# Files that are part of the project
SOURCES = main.c md5.c sqlite3.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = main.exe

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJECTS) core