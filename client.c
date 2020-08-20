
/*
Made By : Anmol Kumar, 2018382

IMPORTANT : You must first execute the server.c file executing this file
*/


#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <pthread.h> 

#define SERVER_FIFO "/tmp/addition_fifo_server"

int fd, fd_server, bytes_read;
char buf1 [512], buf2 [1024];
char newstring[128];
char my_fifo_name [128];
int programflag = 0;

void *send(void *pthread)
{
    
    while(1)
    {
        
        int *myid = (int *)pthread; 

        if(programflag == 0)
        {
            strcat (newstring, my_fifo_name);
            // send message to server
            if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
                perror ("open: server fifo");
                break;
            }

            if (write (fd_server, newstring, strlen (newstring)) != strlen (newstring)) {
                perror ("write");
                break;
            }   

            if (close (fd_server) == -1) {
                perror ("close");
                break;
            }
            programflag = 1;
        }

        else if(programflag ==1)
        {
            printf("\n");
            if (fgets(buf1, sizeof (buf1), stdin) == NULL)
                break;
            strcpy (buf2, my_fifo_name);
            strcat (buf2, " ");
            strcat (buf2, buf1);
            // printf("%s",buf2);

            // send message to server
            if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
                perror ("open: server fifo");
                break;
            }

            if (write (fd_server, buf2, strlen (buf2)) != strlen (buf2)) {
                perror ("write");
                break;
            }   

            if (close (fd_server) == -1) {
                perror ("close");
                break;
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void *receive(void *pthread)
{ 
    while(1)
    {
        char read_data[20][100];
        for(int i =0;i<20;i++)
        {
            strcpy(read_data[i],"");
        }
        
        if ((fd = open (my_fifo_name, O_RDONLY)) == -1)
           perror ("open");

        memset (buf2, '\0', sizeof (buf2));

        if ((bytes_read = read (fd, buf2, sizeof (buf2))) == -1)
            perror ("read");

        if (bytes_read > 0) 
        {
            char* token;
            char* rest = buf2; 
            int i = 0;
            while ((token = strtok_r(rest, " ", &rest)))
            { 
                strcpy(read_data[i],token);
                i++;
            }

            if(strcmp("success",read_data[0])==0)
            {
                printf ("Creation Status: %s \nYour address is: %s\nYour port number is: %s\n", read_data[0],my_fifo_name,read_data[1]);
            }
            else
            {
                // printf("%s",buf2);

                printf("\nMessage from %s: ", read_data[1]);
                for(int i = 2;i<20;i++)
                {
                    printf("%s ",read_data[i]);
                }
            }
        }

        if (close (fd) == -1) {
            perror ("close");
            break;
        }
    }
    pthread_exit(NULL);
    return NULL;

}


int main (int argc, char **argv)
{
    
    // make client fifo with unique pid address
    sprintf (my_fifo_name, "/tmp/add_client_fifo%ld", (long) getpid ());

    // printf("%s",my_fifo_name);

    // making client named pipe
    if (mkfifo (my_fifo_name, 0664) == -1)
        perror ("mkfifo");

    printf("Welcome to the Chat ! ");
    strcpy(newstring,"new ");

    // seperate process threads for sending and receiving
    pthread_t sending_thread,receiving_thread;

    // calling send function in a thread
    if(pthread_create(&sending_thread, NULL, send, (void *)&sending_thread)!=0)
    {
        fprintf(stderr, "Error creating sending thread\n");
        return 1;
    }

    // calling receive function in another thread
    if(pthread_create(&receiving_thread, NULL, receive, (void *)&receiving_thread)!=0)
    {
        fprintf(stderr, "Error creating receiving thread\n");
        return 1;
    }

    //joining both the threads

    if(pthread_join(sending_thread, NULL))
        fprintf(stderr, "Error joining sending thread\n");

    if(pthread_join(receiving_thread, NULL))
        fprintf(stderr, "Error joining receiving thread\n");

    return 0;
}
