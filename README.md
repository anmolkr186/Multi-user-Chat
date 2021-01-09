# Multi-user chat system using Named Pipes

Implementation of IPC between a server and two or more clients usingnamed pipes. In general, ordinary pipes cannot be used for communication between unrelated processes. This can be achieved by using another kind of pipes called Named pipes.

### Code Description

- There are two C files : server.c and client.c. For initiating the communication betweendifferent terminals where we first need to run server.c before executing two or moreclient.c because at least 2 clients are required for a message to be sent and received.
- The server would have the named pipe : addition_fifo_server. Each client would have itsunique named pipe with the nomenclature : add_fifo_client(pid of the current terminalprocess). Both types of pipes are created in /tmp because it has appropriatepermissions.This is done to avoid pipe ambiguity for any number of pipes. The piped is opened usingmkfifo() command with appropriate permissions.
- Two new process threads, sending thread and receiving threads are created so thatboth sending and receiving functions are active during any time of execution of client.c.
- As soon as a process is created, a string is passed to the server named pipe which isparsed and hence the server identifies the creation of a new client. The servermaintains a string array of named pipes of all clients. After that the client process cansend or receive a message simultaneously.
- In send(), a string is passed which contains the sender’s named pipe string, sendingrequirement(either send the message to a particular client or send a message to allclient) and the message itself, the message is then parsed when received by the server.
- Then a new string is formulated which contains the receiver’s named pipe name,sender’s number and the message. The message is then read using read() function ofany client which then prints the message.
- These processes are valid for any number of client processes, provided that a singleserver is online.

### Compilation instructions

- The server.c and client.c files are compiled using the commands:

```bash
> gcc -pthread -o server server.c
> gcc -pthread -o client client.c
```

- We then first need to run the command ./server and after that ./client in any different number of terminals.

### Expected Input

Any client could either send the message to a single client or to all the clients. 

- For singlereceiver client:

    Send (receiver number) (message to be sent)

- For all the receiver clients:

    Send all (message to be sent)

### Expected Output

- When server.c is executed :

```bash
Server Online
```

- When client.c is executed :

```c
Welcome to the Chat !
Creation Status: success
Your address is: /tmp/add_client_fifo2852
Your port number is: 2
```

- Whenever a client sends a message, it is displayed on server terminal as :

```bash
>RECEIVED message from 2: Hello I am Anmol !
```

- Whenever a server sends a message to the receiver client, it is displayed on server terminal as :

```bash
>SENT message to all: Hello I am Anmol !
```

- The received message is then displayed on the receivers’ terminal as :

```bash
Message from 2: Hello I am Anmol !
```

### Reference Links

- https://www.softprayog.in/programming/interprocess-communication-using-fifos-in-linux
- https://www.geeksforgeeks.org/named-pipe-fifo-example-c-program/
- https://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/
- https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_named_pipes.htm
