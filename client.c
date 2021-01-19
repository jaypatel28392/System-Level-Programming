#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080
#define MSG_SIZE 1024
int
main (int argc, char const *argv[])
{
  struct sockaddr_in address;
  int sockfd = 0, valread;
  struct sockaddr_in serv_addr;
  char *hello = "HELLO FROM USER";
  int fd;
  fd_set readfds, testfds, clientfds;
  char msg[MSG_SIZE + 1];
  char kb_msg[MSG_SIZE + 10];
  int result;

  char buffer[1024] = { 0 };
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf ("\n COULD NOT MAKE SOCKET \n");
      return -1;
    }

  memset (&serv_addr, '0', sizeof (serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons (PORT);

  if (inet_pton (AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
      printf ("\n ADDRESS CAN NOT GO THROUGH \n");
      return -1;
    }

  if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) <
      0)
    {
      printf ("\n UNSUCCESSFULL CONNECTION \n");
      return -1;
    }

  fflush (stdout);

  FD_ZERO (&clientfds);

  FD_SET (sockfd, &clientfds);

  FD_SET (0, &clientfds);



  while (1)
    {

      testfds = clientfds;
      select (FD_SETSIZE, &testfds, NULL, NULL, NULL);

      for (fd = 0; fd < FD_SETSIZE; fd++)
	{

	  if (FD_ISSET (fd, &testfds))
	    {
	      if (fd == sockfd)
		{



		  result = read (sockfd, msg, MSG_SIZE);
		  msg[result] = '\0';
		  printf ("%s", msg);

		  if (msg[0] == 'X')
		    {
		      close (sockfd);
		      exit (0);
		    }
		}
	      else if (fd == 0)
		{


		  fgets (kb_msg, MSG_SIZE, stdin);

		  if (strcmp (kb_msg, "quit\n") == 0)
		    {
		      sprintf (msg, " CLIENT IS DISCONNETING \n");
		      write (sockfd, msg, strlen (msg));
		      close (sockfd);
		      exit (0);
		    }
		  else
		    {
		      sprintf (msg, "%s", kb_msg);
		      write (sockfd, msg, strlen (msg));
		    }
		}
	    }
	}
    }
  return 0;
}
