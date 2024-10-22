#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sbb_socket.h"
#include <vector>

using namespace std;

int
main() 
{
	/*
	 *  we'll default to this host and call a section 3 func to get this host
	 */
        char	hostname[128];
	if( gethostname(hostname,sizeof(hostname)) ){
		fprintf(stderr," gethostname(...) failed errno: %d\n", errno);
		exit(1);
	}
	printf("gethostname() returned local hostname: \"%s\"\n", hostname);

	/*
	 * set up socket structure for our host machine
	 */
	struct hostent	*hp;
	if ((hp = gethostbyname(hostname)) == 0) {
		fprintf(stderr,"gethostbyname(...) failed errno: %d exiting...\n", errno);
		exit(1);
	}

	struct sockaddr_in	sock_addr;	/* address in the communications space of the socket */
	memset(&sock_addr, 0, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	sock_addr.sin_port = htons(3039);

	/* 
	 * get an Internet domain socket 
	 */
	int sd;
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr,"socket(...) failed errno: %d exiting...\n", errno);
		exit(1);
	}

	/* 
	 * connect to the PORT 
	 */
	if (connect(sd,(struct sockaddr *)  &sock_addr, sizeof(sock_addr)) == -1) {
		fprintf(stderr,"\ncould not connect to port: %x on server: \"%s\" exiting...\n", 
			PORT,
			hostname);
		exit(1);
	}

	/* 
	 * send a message to the server PORT
	 */
	static char* test_msg = "howdy - this is a message from the client";
	if (send(sd, test_msg, strlen(test_msg), 0) == -1) {
		fprintf(stderr,"send(...) failed - errno: %d\n", errno);
		//exit(1);
	}
	else
	{
		fprintf(stderr,"send(...) of msg: [%s]\n", test_msg);
	}

        /* 
	 * wait for ACK 
	 */
	char	msg[MSGSIZE];
        if (recv(sd, msg, MSGSIZE, 0) == -1) {
		fprintf(stderr,"recv(...) failed errno: %d exiting...\n", errno);
                //exit(1);
        }
	else
	{
		printf(" Got response from server! msg: [%s]\n\n", msg);
	}
    
    //message to get stats back
    static char* get_stats_msg = "get spread var: 99";
    if (send(sd, get_stats_msg, strlen(get_stats_msg), 0) == -1) {
        fprintf(stderr,"send(...) failed - errno: %d\n", errno);
        //exit(1);
    }
    else
    {
        fprintf(stderr,"send(...) of msg: [%s]\n", get_stats_msg);
    }
    

    char	msg2[MSGSIZE];
    if (recv(sd, msg2, MSGSIZE, 0) == -1) {
        fprintf(stderr,"recv(...) failed errno: %d exiting...\n", errno);
        //exit(1);
    }
    else
    {
       //printf("The Hedge Amount, The Total Market Value and the The Total Market Value Given 100 bp Up is:\n");
        
        //printf("The Total Change in Position and the Total Change in LGD is\n");
        
        printf("The spread VaR of the closing book is:\n ");
        char * pch;
        pch = strtok (msg2,"/");
        
        while (pch != NULL){
            printf ("%s\n", pch);
            pch = strtok (NULL, "/");
        }
    }



	close(sd);
}
