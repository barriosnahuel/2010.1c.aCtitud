OBJS1= common.o client.o wclient2.o 
OBJS2= common.o server.o wserver.o
CC=gcc
INCDIR= /usr/local/ssl/include
CFLAGS=  -I$(INCDIR)
LIBDIR=/usr/local/ssl/lib
LIBS= -L$(LIBDIR) -ldl -lssl -lcrypto
PROGS=wclient2 wserver2

all: $(PROGS)
	rm *.o

wclient2: $(OBJS1)
	$(CC) $(OBJS1) $(LIBS) -o wclient2

wserver2: $(OBJS2)
	$(CC) $(OBJS2) $(LIBS) -o wserver2

wserver2.o: wserver2.c
	$(CC) $(CFLAGS) -c wserver2.c -o wserver2.o
	

wclient2.o: wclient2.c
	$(CC) $(CFLAGS) -c wclient2.c -o wclient2.o

common.o: common.c
	$(CC) $(CFLAGS) -c common.c -o common.o

server.o: server.c 
	$(CC) $(CFLAGS) -c server.c -o server.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

clean:
	rm $($PROG) *.o

