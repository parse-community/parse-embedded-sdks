test: tests_run
	./tests_run

tests_run: simplejson_tests.o simplejson.o
	g++ simplejson.o simplejson_tests.o -o tests_run

simplejson_tests.o: simplejson_tests.cc ../simplejson.h
	g++ simplejson_tests.cc -c

simplejson.o: ../simplejson.c ../simplejson.h
	g++ ../simplejson.c -c

