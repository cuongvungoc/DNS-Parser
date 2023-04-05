CC:= gcc
CFLAGS:= -Wall
DEPS:= dns_parser.h
OBJECT:= lock2.o dns_parser.o
TARGET:= lock2

all: $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECT)