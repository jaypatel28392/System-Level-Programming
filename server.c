#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080
#define MAX_CLIENTS 4
#define MSG_SIZE 1024

int
main (int argc, char const *argv[])
{
  int server_fd, client_sockfd;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof (address);
  char buffer[1024] = { 0 };
  char server_message[1024] = "HI THIS IS CHATROOM ! ";

  int fd;
  char fd_array[MAX_CLIENTS];
  char client_names[MAX_CLIENTS][1024];
  fd_set readfds, testfds;
  int num_clients = 0;

  if ((server_fd = socket (AF_INET, SOCK_STREAM, 0)) == 0)
    {
      perror ("SOCKET UNSUCCESSFUL");
      exit (EXIT_FAILURE);
    }


  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (PORT);

  if (bind (server_fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      perror ("BIND UNSUCCESSFUL");
      exit (EXIT_FAILURE);
    }
  if (listen (server_fd, 3) < 0)
    {
      perror ("LISTENING");
      exit (EXIT_FAILURE);
    }

  FD_ZERO (&readfds);
  FD_SET (server_fd, &readfds);
  FD_SET (0, &readfds);

  while (1)
    {
      char msg[1024], kb_msg[1024];
      testfds = readfds;
      select (FD_SETSIZE, &testfds, NULL, NULL, NULL);
      for (fd = 0; fd < FD_SETSIZE; fd++)
	{
	  if (FD_ISSET (fd, &testfds))
	    {

	      if (fd == server_fd)
		{
		  client_sockfd = accept (server_fd, NULL, NULL);

		  if (num_clients < MAX_CLIENTS)
		    {

		      FD_SET (client_sockfd, &readfds);
		      fd_array[num_clients] = client_sockfd;



		      printf ("USER %d HAS JOINED \n SUCCESSFULLY", num_clients);
		      fflush (stdout);

		      sprintf (msg, "%s", "INTRODUCE YOURSELF YOUR, TO START, PLEASE PROVIDE YOUR NAME : ");
		      send (client_sockfd, msg, strlen (msg), 0);
		      num_clients++;

		    }
		  else
		    {
		      sprintf (msg,
			       "SORRY THE SERVER IS CURRENTLY FULL \n");
		      write (client_sockfd, msg, strlen (msg));
		      close (client_sockfd);
		    }
		}
	      else if (fd == 0)
		{		
		  fgets (kb_msg, MSG_SIZE + 1, stdin);

		  if (strcmp (kb_msg, "quit\n") == 0)
		    {

		      sprintf (msg, "SERVER IS GOING OFFLINE.\n");

		      for (int i = 0; i < num_clients; i++)
			{
			  write (fd_array[i], msg, strlen (msg));
			  close (fd_array[i]);
			}
		      close (server_fd);
		      exit (0);
		    }
		  else
		    {
		      sprintf (msg, "SERVER'S MESSAGE:%s", kb_msg);
		      for (int i = 0; i < num_clients; i++)
			write (fd_array[i], msg, strlen (msg));
		    }
		}
	      else if (fd)
		{
		  printf ("SERVER BACKUP MESSAGE\n");

		  int result = read (fd, msg, MSG_SIZE);

		  if (result == -1)
		    perror ("read()");
		  else if (result > 0)
		    {
		      char newmsg[1024];
		      int clientid = 0;

		      msg[result] = '\0';
		      printf (msg);
		      for (int i = 0; i < num_clients; i++)
			{
			  if (fd_array[i] == fd)
			    {
			      clientid = i;
			    }

			}
		      sprintf (newmsg, "MESSAGE FROM USER %d : %s", clientid, msg);
		      for (int i = 0; i < num_clients; i++)
			{
			  if (fd_array[i] != fd)
			    {
			      sprintf (newmsg, "MESSAGE FROM USER %d : %s", clientid,
				       msg);
			      write (fd_array[i], newmsg, strlen (newmsg));
			    }


			}


		    }
		  else
		    {
		      int clientid = 0;
		      for (int i = 0; i < num_clients; i++)
			{
			  if (fd_array[i] == fd)
			    {
			      clientid = i;
			    }
			}
		      close (fd);
		      FD_CLR (fd, &readfds);
		      printf ("USER %d HAS LEFT THE SERVER \n", clientid);
		      num_clients--;
		      fflush (stdout);

		    }

		}
	    }
	}
    }


  close (server_fd);
  return 0;
}
