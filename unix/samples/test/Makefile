CC = gcc
SRCS = main.c simplejson.c
OBJS = $(SRCS:.c=.o)
INCLUDES = -I../../../common/
LIBDIRS =
LIBS = -lparse
TARGET = parse_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LIBDIRS) $(OBJS) -o $(TARGET) $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^  -o $@

simplejson.o: ../../../common/simplejson.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^  -o $@

clean:
	rm -fR $(TARGET) $(OBJS)
