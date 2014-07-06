/*
LUS weather_client tries to connect with server in Coldipozzo on port 5005

*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
 
int main(int argc , char *argv[])
{
	int rtn;
	while (1)
	{
		rtn = connect_to_Coldipozzo();
		sleep(600);
	}
}


int connect_to_Coldipozzo()
{
    	int sock;
    	DIR *dirp;
    	FILE *in;
    	struct dirent *dp1;
    	struct sockaddr_in server;
    	struct addrinfo hints, *res;
    	char message[100],*s_rtn,line[50],file_name[300];
	
    	memset(&hints, 0, sizeof hints);
    	hints.ai_family = AF_UNSPEC;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;
 
    	getaddrinfo("coldipozzo.homelinux.com", "5005", &hints, &res);
    	//Create socket
    	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    	if (sock == -1)
    	{
        	printf("Could not create socket");
    	}
    	puts("Socket created");
     
 
    	//Connect to remote server
    	if (connect(sock , res->ai_addr, res->ai_addrlen) < 0)
    	{
        	perror("connect failed. Error");
        	return 1;
    	}
    	puts("Connected\n");
 	dirp = opendir("/mnt/1wire/");
        while ((dp1 = readdir(dirp)) != NULL)
        {
                s_rtn = strstr(dp1->d_name,"28.");
                if (s_rtn != NULL)
       		{
			strcpy(file_name,"/mnt/1wire/");
			strcat(file_name,dp1->d_name);
			strcat(file_name,"/temperature12");	//this is 12 bit resolution
                	in = fopen(file_name,"r");
                	if (in != NULL)
                	{
                         	fgets( line, sizeof line, in);
                         	//printf("%s",line);
				fclose(in);
				message[0] = '\0';
				strcpy(message,dp1->d_name);
				strcat(message,"\t");
				strcat(message,line);
				strcat(message,"\n");
				printf("Message sent: %s",message);
       				if( send(sock , message , strlen(message) , 0) < 0)
       				{
       					puts("Send failed");
       					return (1);
       				}
			} else {
			printf("Unable to open %s\n",file_name);
			}
		}
    	}
    	close(sock);
    	return (0);
}
