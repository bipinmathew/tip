tip: tip.o libtip.so
	echo Making tip...

tip.o: tip.c tip.h
	gcc -g -c -Wall -Werror -fPIC tip.c -o tip.o

libtip.so: tip.o
	gcc -g -shared -o libtip.so tip.o

main: main.c libtip.so
	gcc -L. -Wl,-rpath=`pwd .` -Wall -o main main.c -ltip

docs: tip doxygen.config
	doxygen doxygen.config	

clean:
	rm -rf tip.o libtip.so main main.o ./docs/*

all: tip docs
