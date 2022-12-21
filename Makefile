all: mystack Receiver Sender receiver

mystack:
	gcc mystack.c -c -Wall	

receiver:
	gcc -o Receiver Receiver.o mystack.o -Wall 		

Receiver:
	gcc Receiver.c -c -Wall 

Sender:
	gcc -o Sender Sender.c -Wall 	

clean:
	rm -f *.o Sender Receiver

