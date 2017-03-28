main: define.c server.c client.c
	gcc -Wall -Wno-unused-result -O2 -pthread -o bin/server server.c
	gcc -Wall -lpthread -pthread -o bin/client client.c

clean:
	rm -f bin/server
	rm -f bin/client
