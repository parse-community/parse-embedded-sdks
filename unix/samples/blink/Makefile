CC = gcc
SRCS = main.c gpio.c json.c
OBJS = $(SRCS:.c=.o)
INCLUDES =
LIBDIRS =
LIBS = -lparse -lrt
TARGET = blink 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LIBDIRS) $(OBJS) -o $(TARGET) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	rm -fR $(TARGET) $(OBJS)
