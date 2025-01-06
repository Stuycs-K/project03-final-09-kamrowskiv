compile: client1 server1

client1: client.o
	@gcc -o clientfile client.o

client.o: client.c shared.h
	@gcc -c client.c

play: clientfile
	@./clientfile

server1: server.o
	@gcc -o serverfile server.o

server.o: server.c shared.h
	@gcc -c server.c

server: serverfile
	@./serverfile

clean:
	@rm *.o
	@rm *~
