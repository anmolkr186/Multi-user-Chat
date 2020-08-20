
/*
Made By : Anmol Kumar, 2018382
*/

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

// server named pipe 
#define SERVER_FIFO "/tmp/addition_fifo_server"

int main (int argc, char **argv)
{
    
    int fd, fd_client, bytes_read, i;
    char fifo_name_set[20][100];
    int counter = 0;
    char return_fifo[20][100];
    char buf [4096];

    printf("Server Online\n");

    // array of strings maintaining addresses of client terminals
    for(int i = 0;i<20;i++)
    {
        strcpy(fifo_name_set[i],"");
    }

    // making fifo
    if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
        perror ("mkfifo");
        exit (1);
    }
    // opening fifo
    if ((fd = open (SERVER_FIFO, O_RDONLY)) == -1)
        perror ("open");

     while(1)
     {

        strcpy(return_fifo[0],"\0");
        
        for(int i =0;i<20;i++)
        {
            strcpy(return_fifo[i],"");
        }

         // setting memset for filling a block of memory for string type buf    
         memset (buf, '\0', sizeof (buf));

         if ((bytes_read = read (fd, buf, sizeof (buf))) == -1)
             perror ("read");

         if (bytes_read > 0) 
         {
            // printf("Buf %s",buf);
           
            // string parsing
            char* token;
            char* rest = buf; 
            int i = 0;
            while ((token = strtok_r(rest, " ", &rest)) && strlen(token)!=0)
            { 
                strcpy(return_fifo[i],token);
                i++;
            }

            int sender_number = 0;
            
            // adding new client address to fifo_name_set
            if(strcasecmp(return_fifo[0],"new")==0)
            {
                // printf("\nNew user added with address: %s", return_fifo[1]);                
                counter++;
                strcpy(fifo_name_set[counter],return_fifo[1]);
                sender_number = counter;
       
                char* str = return_fifo[1];
                char success_string[100];
                char a[5];
                sprintf(a,"%d",sender_number);

                strcpy(success_string,"success ");
                strcat(success_string,a);

                // sending the creatin status back to client
                if ((fd_client = open (str, O_WRONLY)) == -1) {
                    perror ("open: client fifo");
                    continue;
                }

                if (write (fd_client, success_string, strlen (success_string)) != strlen (success_string))
                    perror ("write");

                if (close (fd_client) == -1)
                    perror ("close");
            }
            

            // sending messages to clients
            else if(strcasecmp(return_fifo[1],"send")==0)
            {
                // parsing the message and the sender

                for(int i =1;i<20;i++)
                {
                    if(strcasecmp(return_fifo[0],fifo_name_set[i])==0)
                    {
                        sender_number = i;
                    }   
                }
                printf("\n>RECIEVED message from %d: ", sender_number);
                // printf("\nMessaged recieved:");
                for ( int i=3;i<20;i++)
                    printf("%s ",return_fifo[i]);

                
                // sending to all clients
                if(strcasecmp(return_fifo[2],"all")==0)
                {
                    char a[5];
                    
                    //sending to all the clients

                    for(int j =1;j<20;j++)
                    {
                        if(strlen(fifo_name_set[j])!=0)
                        {

                            char currentClient[100],receiver_address[500]="";
                            strcpy(currentClient,fifo_name_set[j]);
                            strcpy(receiver_address,currentClient);


                            // searching for sender's index from fifo_name_set and adding to reciever's address
                            for(int i =1;i<20;i++)
                            {
                                if(strcasecmp(return_fifo[0],fifo_name_set[i])==0)
                                {
                                    sprintf(a,"%d",i);
                                    strcat(receiver_address," ");
                                    strcat(receiver_address,a);
                                    break;
                                }
                            } 

                            // adding message
                            for(int i =3;i<20;i++)
                            {
                                if(strlen(return_fifo[i])!=0)
                                {
                                    strcat(receiver_address," ");
                                    strcat(receiver_address,return_fifo[i]);
                                }
                            }

                            if(strcasecmp(return_fifo[0],currentClient)!=0)
                            {
                                // printf("\nReciever address : %s", receiver_address);
                                
                                
                                
                                //sending the message
                                if ((fd_client = open (currentClient, O_WRONLY)) == -1)
                                {
                                    perror ("open: client fifo");
                                    continue;
                                }

                                if (write (fd_client, receiver_address, strlen (receiver_address)) != strlen (receiver_address))
                                    perror ("write");

                                if (close (fd_client) == -1)
                                    perror ("close");
                            }

                            printf("\n>SENT message to all: ");
                            for ( int i=3;i<20;i++)
                                printf("%s ",return_fifo[i]);                            
                           
                        }
                        
                    }

                }

                // sending to a particular client
                else
                {
                    char receiver_address[200]="";

                    int val = atoi(return_fifo[2]);
                    char a[5];

                    if(strcasecmp(fifo_name_set[val],return_fifo[0])!=0)
                    {
                        strcpy(receiver_address,fifo_name_set[val]);
                        
                        for(int i =1;i<20;i++)
                        {
                            if(strcasecmp(return_fifo[0],fifo_name_set[i])==0)
                            {
                                sprintf(a,"%d",i);
                                strcat(receiver_address," ");
                                strcat(receiver_address,a);
                                break;
                            }
                        } 

                        for(int i =3;i<20;i++)
                        {
                            if(strlen(return_fifo[i])!=0)
                            {
                                strcat(receiver_address," ");
                                strcat(receiver_address,return_fifo[i]);
                            }
                            
                        }

                        // printf("\nReciever address : %s", receiver_address);
                        printf("\n>SENT message to %s: ",return_fifo[2]);
                        for ( int i=3;i<20;i++)
                            printf("%s ",return_fifo[i]);
                    }
                        
                    /* Send the result to receiver client*/
                    if ((fd_client = open (fifo_name_set[val], O_WRONLY)) == -1) {
                        perror ("open: client fifo");
                        continue;
                    }

                    if (write (fd_client, receiver_address, strlen (receiver_address)) != strlen (receiver_address))
                        perror ("write");

                    if (close (fd_client) == -1)
                            perror ("close");

                }

            }
            
        }

     }
}