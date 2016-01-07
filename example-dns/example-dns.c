#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<errno.h>
#include<netdb.h>
#include<arpa/inet.h>

int hostname_to_ip(char *  , char *);

int main(int argc , char *argv[])
{
	if(argc <2)
	{
		printf("usage : command domain-address\n");
		exit(1);
	}
	
	char *hostname = argv[1];
	char ip[100];
	
	hostname_to_ip(hostname , ip);
	printf("%s resolved to %s" , hostname , ip);
	
	printf("\n");
	
}

/*
  Get ip from domain name
*/

int hostname_to_ip(char * hostname , char* ip)
{
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
	
	if ( (he = gethostbyname( hostname ) ) == NULL) 
	{
		// get the host info
		herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i = 0; addr_list[i] != NULL; i++) 
	{
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
		return 0;
	}
	
	return 1;
}

