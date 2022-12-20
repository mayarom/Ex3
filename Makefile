# Makefile for TCP project
# Author: 2015-2016, Jack Lange <
# Date: 2015-11-01
#all: mystack tcp_receiver tcp_sender 

#-c compile .o files use this when i want to compile more than one file 
#-o change the name of the output file - only when i want to chane to another name
#Wall is a flag that tells the compiler to print all warnings
all: mystack tcp_receiver receivermain tcp_sender

tcp_receiver: 
	gcc tcp-receiver.c -c -Wall 

tcp_sender: 
	gcc -o Sender tcp-sender.c -Wall

mystack: 
	gcc mystack.c -c -Wall

receivermain:
	gcc -o Receiver mystack.o tcp-receiver.o -Wall

clean: 
	rm -f *.o 
