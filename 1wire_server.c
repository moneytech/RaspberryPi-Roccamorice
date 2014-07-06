//reworked to have multiple connections-
//select to prevent hanging-
//direct lookup of all devices
//LUS early fall morning 10-3-2013



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
//#include <pthread.h>


#define DIRECTORY "/mnt/1wire/"
#define MAX_DEV	20
#define MAX_NAME 60
#define STRLEN	35
#define TEMPERATURE	1
#define HUMIDITY	2
#define BAROMETER	3
#define SWITCH	4

//copied to serverthread
#define MAX_LINE           200
#define LISTENQ        (1024)
#define VALID_TT	300	//300 seconds for reading to remain valid ie 5 min

#define BACKLOG 10     // how many pending connections queue will hold
#define PORT	"18300"




//char Valid_dir[MAX_DEV][MAX_NAME];
//char Valid_dir[MAX_NAME][MAX_DEV];
//int Type[MAX_DEV];
//int Ptr=0;

int Debug = 0;
int Debug_out = 0;
int Debug_LCD = 0;
int Storing = 0;
char String[80];

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int connect_loop()
{
    	//int       list_s;                /*  listening socket          */
    	//int       conn_s;                /*  connection socket         */
    	//short int port;                  /*  port number               */
    	//struct    sockaddr_in servaddr;  /*  socket address structure  */
    	//char      buffer[MAX_LINE];      /*  character buffer          */
    	//char     *endptr;                /*  for strtol()              */
	//int 	i,rtn,storing,act;
	//char	*s_ptr,tmp[20],name[80];
	//FILE	*out;
	//char	device[40],offset[40],rw[10];


	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	int rtn,storing;
    	short int port;                  /*  port number               */

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  
	{
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) 
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) 
	{
		perror("sigaction");
		exit(1);
	}

	//printf("server: waiting for connections on port %s...\n",PORT);
	storing = 0;

	while(1) 	
	{  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) 
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		//printf("server: got connection from %s\n", s);

		if (!fork()) 
		{ // this is the child process
			close(sockfd); // child doesn't need the listener
			rtn = read_data(new_fd); //get the request and send back the answer....
			close(new_fd);
			exit(0);	//this fork is finished so die!
		}
		close(new_fd);  // parent doesn't need this
		//loop back to while....and wait for another connection
	}
	//return 0;
//end of Beej's code
}


int read_data(int file_fd)
{
        char      buffer[MAX_LINE];
        char     *endptr;
        int     i,rtn,storing,act,j;
        char    *s_ptr,tmp[20],name[80];
        FILE    *out;
        char    device[40],offset[40],rw[100],message[100];
	char	file_name[120];

	rtn = Readline(file_fd, buffer, MAX_LINE-1);
	//if (Debug) printf("Request: %s\n",buffer);
	//if (Debug_LCD) printf("Request: %s\n",buffer);
	//for (i=0;i<strlen(buffer);i++)	//pppppddddd
	//{
		//printf("%c\t%d\n",buffer[i],buffer[i]);
	//}
	//make all strings as null
	rw[0] = 0;
	device[0] = 0;
	offset[0] = 0;
	sscanf(buffer,"%s\t%s\t%s",device,offset,rw);
	for (i=0;i<strlen(rw);i++)
	{
		if (rw[i] == '~') rw[i] = 32;
	}
	//if (Debug) printf("Start of  parsing: device:%s\toffset:%s\trw:%s\n",device,offset,rw);
	//if (Debug_LCD) printf("Start of  parsing: device:%s\toffset:%s\trw:%s\n",device,offset,rw);
	if (rw[0] == 0)	//no write request only a read
	{
		strcpy(buffer,device);
		strcat(buffer,"\t");


		strcpy(file_name,DIRECTORY);
		strcat(file_name,device);
		strcat(file_name,"/");
		rtn = strcmp(offset,"temperature");
		if (rtn == 0)
		{
			//if (Debug) printf("Got a temperature request");
			strcat(file_name,offset);
		}
		rtn = strcmp(offset,"humidity");
		if (rtn == 0)
		{
			//if (Debug) printf("Got a humidity request");
			strcat(file_name,offset);
		}
		rtn = strcmp(offset,"barometer");
		if (rtn == 0)
		{
			//if (Debug) printf("Got a barometer request");
			//strcat(file_name,offset);
			strcat(file_name,"/TAI8570/pressure");
		}
		s_ptr = strstr(offset,"PIO.");
		if (s_ptr != NULL)	//it a read for PIO.0-7
		{
			//if (Debug) printf("Got a PIO request");
			strcat(file_name,offset);
		}

		read_file(file_name);
		strcat(buffer,String);
		strcat(buffer,"\n");
		if (Debug) printf("Sending Return string is %s\n",buffer);
		//wwwwwwwww
		Writeline(file_fd, buffer, strlen(buffer));
	} else {	//rw not NULL
		if (Debug_out) printf("rw is not null:rw[0] = %d\t%s\n",rw[0],rw);
		//if (Debug_LCD) printf("rw is not null:rw[0] = %d\t%s\n",rw[0],rw);
		s_ptr = strstr(offset,"PIO");
		if (s_ptr != NULL)
		{
			//PIO 4-7
			strcpy(file_name,DIRECTORY);
			strcat(file_name,device);
			strcat(file_name,"/");
			strcat(file_name,offset);
			//strcat(file_name,"/");
			if (Debug_out) printf("PIO file_name: %s\n",file_name);
			out = fopen(file_name,"w");
			if (out == NULL)
			{
				if (Debug_out) printf("Can not open %s\n",file_name);
				return(-1);
			} else {
				if (toupper(rw[1]) == 'N' )
				{
					fprintf(out,"1\n");
				} else {
					fprintf(out,"0\n");
				}
				fclose(out);
			}
		}
		if (rw[0] == 34)	//34 is "
		{
			if(Debug_out) printf("got a LCD string %s\n",rw);
			//if (Debug_LCD) printf("rw is not null:rw[0] = %d\t%s\n",rw[0],rw);
			if (isdigit(rw[1]))
			{
				if (Debug_LCD) printf("Will write message to screen\t%s\n",rw);
				strcpy(file_name,DIRECTORY);
				strcat(file_name,device);
				strcat(file_name,"/");
				strcat(file_name,"LCD_H/");
				strcat(file_name,"screenyx");
				if (Debug_LCD) printf("message file name = %s\n",file_name);
				out = fopen(file_name,"w");
                        	if (out == NULL)
                        	{
                                	if (Debug_LCD) printf("Can not open %s\n",file_name);
                                	return(-1);
                        	} else {
					j = 0;
					for (i=0;i<strlen(rw);i++)
					{
						if (rw[i] != 34)
						{
							message[j] = rw[i];
							j++;
						}
					}
					message[j] = 0;
					if (Debug_LCD) printf("Message is %s\n",message);	
					fprintf(out,"%s",message);
					fclose(out);
				}
			}
			if (rw[1] == 'c')	//clear
			{
				if (Debug_LCD) printf("Will clear the screen\n");
                                if (Debug_LCD) printf("Will write message to screen\t%s\n",rw);
                                strcpy(file_name,DIRECTORY);
                                strcat(file_name,device);
                                strcat(file_name,"/");
				strcat(file_name,"LCD_H/");
				strcat(file_name,"clear");
				 out = fopen(file_name,"w");
                                if (out == NULL)
                                {
                                        if (Debug_LCD) printf("Can not open %s\n",file_name);
                                        return(-1);
                                } else {
					fprintf(out,"%s","1");
                                        fclose(out);
                                }       

			}
		}
	}
	return(0);
}


int read_file(char *file_name)
{
	FILE *in;
	char str[80];	
	int i,j;
	in = fopen (file_name,"r");
	if (in == NULL)
	{
		printf("unable to open %s\n",file_name);
		return(-1);
	} else {
		fgets(str,STRLEN - 1,in);
		if (Debug) printf("read_file: string returned from file read %s\n",str);
		if (str[0] == 0)
		{
			if (Debug) printf ("Error in reading %s\n",file_name);
			return(-1);
		} else {
			j=0;
			for (i=0;i<strlen(str);i++)
			{
				if (str[i] !=32) 
				{
					String[j]=str[i];
					j++;
				}
			}
			String[j] = 0;
			//if (Debug) printf("String = %s strlen=%d\n",String,strlen(String));
		}
		fclose(in);
	}
	return(0);
}
				



main()
{
	//Debug = 1;
	//Debug_out = 1;
	Debug_LCD = 1;
	connect_loop();
}


/*
			if (Devices[i].function == BAROMETER)	//bbbbbb
			{
				strcat(name,"/");
				strcat(name,"TAI8570/pressure");
				if (Debug) printf("Looking up %s\n",name);
				IN = fopen(name,"r");
				if (IN == NULL)
				{
					printf("Error in trying to read %s\n",name);
				} else {	//bbbbbb
					f_temp = -1;
					str[0] = 0;
					fgets(str,STRLEN - 1,IN);
					if (str[0] == 32)
					{
						if (Debug) printf("Barometer strlen(str)=%d\n",strlen(str));
						for (j=0;j<strlen(str);j++)
						{
							if (str[j] != 32)
							{
								Devices[i].cvalue[k] = str[j];
								k++;
							}
							Devices[i].cvalue[k+1] =0;
						}	
					}
					if (Debug) printf("Read string value of %s\n",str);
					sscanf(str,"%f",&f_temp);
					if (Debug) printf("Barometer: f_temp = %f\n",f_temp);
					if (f_temp > 0) 
					{
						if (Debug) printf("Stored %f!\n",f_temp);
						Devices[i].fvalue = f_temp;
						Devices[i].valid = 1;
					} else {
						if ((tt - Devices[i].valid_tt) > VALID_TT)
						{
							Devices[i].valid = 0;
							if (Debug) printf("Device %s marked not valid\n",Devices[i].address);
						}
					}
					fclose(IN);
				}
			}
			if (Devices[i].function == HUMIDITY)
			{
				strcat(name,"/");
				strcat(name,"humidity");
				if (Debug) printf("Looking up %s\n",name);
				IN = fopen(name,"r");
				if (IN == NULL)
				{
					printf("Error in trying to read %s\n",name);
				} else {	//hhhhhh
					f_temp = -1;
					str[0] = 0;
					fgets(str,STRLEN - 1,IN);
					if (str[0] == 32)
					{
						for (j=0;j<strlen(str);j++)
						{
							if (str[j] != 32)
							{
								Devices[i].cvalue[k] = str[j];
								k++;
							}
							Devices[i].cvalue[k+1] =0;
						}	
					}
					if (Debug) printf("Read string value of %s\n",str);
					sscanf(str,"%f",&f_temp);
					if (Debug) printf("Humidity: f_temp = %f\n",f_temp);
					if (f_temp > 0) 
					{
						if (Debug) printf("Stored %f!\n",f_temp);
						Devices[i].fvalue = f_temp;
						Devices[i].valid = 1;
					} else {
						if ((tt - Devices[i].valid_tt) > VALID_TT)
						{
							Devices[i].valid = 0;
							if (Debug) printf("Device %s marked not valid\n",Devices[i].address);
						}
					}
					fclose(IN);
				}
			}
			if (Devices[i].type[0] =='P')
			{
				if (Devices[i].type[1] =='I')
				{
					if (Devices[i].type[2] =='O')
					{
						if (Devices[i].type[3] =='.')
						{
							ch = Devices[i].type[4];
							ch = ch - '0';
							strcat(name,"/");
							strcat(name,Devices[i].type);
							if (Debug) printf("PIO file name is %s\n",name);
							IN = fopen(name,"r");
                                			if (IN == NULL)
                                			{
                                        			printf("Error in trying to read %s\n",name);
                                			} else {        //PIOPIO
								fgets(str,STRLEN - 1,IN);
								if (Debug) printf("%s input is %s\n",Devices[i].type,str);
								sscanf(str,"%s",response);
								strcpy(Devices[i].cvalue,str);
								p_char = strstr(response,"1");
								if (p_char !=NULL)
								{
									Devices[i].ivalue = 1;
									Devices[i].valid = 1;
									Devices[i].valid_tt = tt;
								}
								p_char = strstr(response,"0");
								if (p_char !=NULL)
								{
									Devices[i].ivalue = 0;
									Devices[i].valid = 1;
									Devices[i].valid_tt = tt;
								}
							}	
							if (ch > -1 && ch < 4)
							{
								if (Debug) printf("PIO.%d is a read requet from input....\n",ch);
							}
							if (ch > 3 && ch < 8)
							{
								if (Debug) printf("PIO.%d is a output\n",ch);
							}

						}
					}
				}
			}
		}
		sleep(10);
	}
				
}
*/

ssize_t Readline(int sockd, void *vptr, size_t maxlen) 
{
	ssize_t n, rc;
	char    c, *buffer;
	buffer = vptr;
	for ( n = 1; n < maxlen; n++ ) 
	{
		if ( (rc = read(sockd, &c, 1)) == 1 ) 
		{
	    		*buffer++ = c;
			//if (Debug_LCD) printf("Readline: received %c\n",c);
	    		if ( c == '\n' ) break;
		}
		else if ( rc == 0 ) 
		{
	    		if ( n == 1 ) return 0;
	    		else break;
		}
		else 
		{
			if ( errno == EINTR ) continue;
			return -1;
		}
	}
	*buffer = 0;
	return n;
}


/*  Write a line to a socket  */

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
    size_t      nleft;
    ssize_t     nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 ) {
	if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
	    if ( errno == EINTR )
		nwritten = 0;
	    else
		return -1;
	}
	nleft  -= nwritten;
	buffer += nwritten;
    }

    return n;
}

