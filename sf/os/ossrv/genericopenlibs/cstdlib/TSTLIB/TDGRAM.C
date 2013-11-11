/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Test of AF_LOCAL datagram sockets, based on Datagram example in GCC.HELP
* 
*
*/



#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef __X86GCC__  
#undef WIN32  // Seems to be included in the X86 baseport somewhere (we don't want winsock as this isn't an emulator build)
#endif

#ifdef WIN32
	#include <winsock.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <libc/netinet/in.h>
	#include <libc/arpa/inet.h>
#endif
int
make_named_socket (int port)
{
//  struct sockaddr name;
  struct sockaddr_in name;
  int sock;
  size_t size;

  /* Create the socket. */

  sock = socket (AF_INET, SOCK_DGRAM, 0);
#ifdef WIN32
	if ( INVALID_SOCKET == sock )
	{
		printf( "%s,%s,%d","CreateSocket", "socket", WSAGetLastError ( ) );
		  exit (EXIT_FAILURE);
	}
#else
	if (-1 == sock )
	{
      perror ("make_named_socket");
		  exit (EXIT_FAILURE);
	}
#endif

  /* Bind a name to the socket. */

  name.sin_family = AF_INET;
  name.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
  name.sin_port = htons((unsigned short)port);

  size = sizeof(struct sockaddr);

  if (bind (sock, (struct sockaddr *) &name, size) < 0)
    {
      perror ("bind");
      exit (EXIT_FAILURE);
    }

  return sock;
}

#define SERVER  99
#define MAXMSG  512


int server(void)
	{
	int server_sock;
	char server_message[MAXMSG];
	struct sockaddr name;
	size_t size;
	int nbytes = -1;

	/* Make the socket, then loop endlessly. */

	server_sock = make_named_socket (SERVER);
	nbytes = sendto (server_sock, server_message, nbytes, 0, (struct sockaddr *) & name, size);
	if (nbytes < 0)
		{
		perror ("sendto (server)");
		exit (EXIT_FAILURE);
		}
	while (1)
		{
		/* Wait for a datagram. */
		size = sizeof (name);
		nbytes = recvfrom (server_sock, server_message, MAXMSG, 0, (struct sockaddr *) & name, &size);
		if (nbytes < 0)
			{
			perror ("recfrom (server)");
			break;
			}

		/* Give a diagnostic message. */
		fprintf (stderr, "Server: got message: %s\n", server_message);

		/* Bounce the message back to the sender. */
		nbytes = sendto (server_sock, server_message, nbytes, 0, (struct sockaddr *) & name, size);
		if (nbytes < 0)
			{
			perror ("sendto (server)");
			break;
			}
		}/* end of - "while (1)" */
	exit (EXIT_FAILURE);
	return 0;
	}

#define CLIENT  125
#define MESSAGE "Yow!!! Are we having fun yet?!? I'd like a 99 please..."

int
client (void)
{
  int sock;
  char message[MAXMSG];
  struct sockaddr_in name;
  size_t size;
  int nbytes;

  /* Make the socket. */
  sock = make_named_socket (CLIENT);

  /* Initialize the server socket address. */
  name.sin_family = AF_INET;
  name.sin_port = htons((unsigned short)SERVER);
  size = sizeof(name);

  /* Send the datagram. */
  nbytes = sendto (sock, MESSAGE, strlen (MESSAGE) + 1, 0,
                   (struct sockaddr *) & name, size);
  if (nbytes < 0)
    {
      perror ("sendto (client)");
      exit (EXIT_FAILURE);
    }

  /* Wait for a reply. */
  nbytes = recvfrom (sock, message, MAXMSG, 0, NULL, 0);
  if (nbytes < 0)
    {
      perror ("recfrom (client)");
      exit (EXIT_FAILURE);
    }

  /* Print a diagnostic message. */
  fprintf (stderr, "Client: got message: %s\n", message);

  /* Clean up. */
#ifdef WIN32
    closesocket (sock);
#else
	close(sock);
#endif
	return 0;
}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1046
@SYMTestCaseDesc	    Tests for AF_LOCAL datagram sockets
@SYMTestPriority 	    High
@SYMTestActions  	    Initialize the server socket address,send and receive messages on the socket.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int both_together(void)
{
  int server_sock;
  char server_message[MAXMSG];
  struct sockaddr_in name;
  size_t size;
  int nbytes;
  int sock;
  char message[MAXMSG];

  /* Make the server socket */

  server_sock = make_named_socket (SERVER);

  sock = make_named_socket (CLIENT);

  /* Initialize the server socket address. */
  name.sin_family = AF_INET;
  //name.sin_port = SERVER;
  name.sin_port = htons((unsigned short)SERVER);
	  name.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
 size = sizeof(name);

  /* Send the datagram. */
  nbytes = sendto (server_sock, MESSAGE, strlen (MESSAGE) + 1, 0,
                   (struct sockaddr *) & name, size);


#ifdef WIN32
  if (nbytes < 0)
	{
		printf( "%s,%s,%d","sendto", "socket", WSAGetLastError ( ) );
		  exit (EXIT_FAILURE);
	}
#else
  if (nbytes < 0)
	{
      perror ("sendto (server)");
		  exit (EXIT_FAILURE);
	}
#endif
  {
  /* more server stuff */

      size = sizeof (name);
      nbytes = recvfrom (server_sock, server_message, MAXMSG, 0,
                         (struct sockaddr *) & name, &size);
#ifdef WIN32
      if (nbytes < 0)
	{
		printf( "%s,%s,%d","recvfrom", "socket", WSAGetLastError ( ) );
		  exit (EXIT_FAILURE);
	}
#else
      if (nbytes < 0)
	{
      perror ("recvfrom (server)");
		  exit (EXIT_FAILURE);
	}
#endif

      /* Give a diagnostic message. */
      fprintf (stderr, "Server: got message: %s\n", server_message);

      /* Bounce the message back to the sender. */
  name.sin_port = htons((unsigned short)CLIENT);
      nbytes = sendto (sock, server_message, nbytes, 0,
                       (struct sockaddr *) & name, size);
#ifdef WIN32
      if (nbytes < 0)
        {
		printf( "%s,%s,%d","sendto", "socket", WSAGetLastError ( ) );
          exit (EXIT_FAILURE);
        }
#else
      if (nbytes < 0)
	{
      perror ("sendto (client)");
		  exit (EXIT_FAILURE);
	}
#endif
  }

  /* Wait for a reply. */
  nbytes = recvfrom (sock, message, MAXMSG, 0, NULL, 0);
#ifdef WIN32
      if (nbytes < 0)
        {
		printf( "%s,%s,%d","recvfrom", "socket", WSAGetLastError ( ) );
          exit (EXIT_FAILURE);
        }
#else
      if (nbytes < 0)
	{
      perror ("recvfrom (client)");
		  exit (EXIT_FAILURE);
	}
#endif

  /* Print a diagnostic message. */
  fprintf (stderr, "Client: got message: %s\n", message);

  /* Clean up. */
#ifdef WIN32
  closesocket (sock);
  closesocket(server_sock);
#else
  close(sock);
  close(server_sock);
#endif

  return 0;
}

int main(int argc, char*argv[])
	{
#ifdef WIN32
	WORD wVersionRequested;WSADATA wsaData;int err; 
	wVersionRequested = MAKEWORD( 1, 0 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
#endif
	return both_together();
}
