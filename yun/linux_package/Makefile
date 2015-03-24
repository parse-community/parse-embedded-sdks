TST=tests
SRC=src
INSTALL_DIR=/usr/bin/

all:  parse_request parse_push
parse: parse_request 

parse_request:
	$(CC) $(CFLAGS) $(SRC)/parse_request.c $(SRC)/yun.c $(SRC)/client.c $(SRC)/utils.c -o parse_request $(LDFLAGS) -lssl -lparse

parse_push:
	$(CC) $(CFLAGS) $(SRC)/parse_push.c $(SRC)/yun.c $(SRC)/client.c $(SRC)/utils.c -o parse_push $(LDFLAGS) -lssl -lparse

install:
	cp parse_request $(INSTALL_DIR) & cp parse_push $(INSTALL_DIR)

clean:
	rm -fR $(TST)/utils.o & rm $(TST)/utils_test.o & rm $(TST)/parse_test & rm parse_request & rm parse_push
