#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <resolv.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define HOST "coding.debuntu.org"
#define PAGE "/"
#define MAXBUF 1024
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT" //In order to construct the time format for the ' If- Modified-Since ' header

int create_tcp_socket();

int main(int argc, char *argv[])
{	
	char* timeInterval; //-h <time_interval> parameter
	int responseHeader = 0, If_Modified_Since = 0; //"If-Modified-Since:" flag
	int day = 0, hour = 0, min = 0, PORT = 8080;
	time_t now;
	char timebuf[128];
	static char* b_url = NULL;
	int argnumP=1;
	//Error handling:
	if(argc>5 || argc<2)
	{
		perror("Usage: client [–h] [–d <time-interval>] <URL>\n");
		return 1;
	}
	int n;
	
	for(n = 1; n < argc; n++)
	{
		//Search of str "-h"
		if(strchr(argv[n], '-'))
		{
			argnumP = 2;
			//printf("without <-h>\n");
		}
		else
		{
			responseHeader = 1;
			//printf("<-h> recieved\n");
			break;
		}
	}
	//Search of str "-d"
	for(n = 1; n < argc; n++)
	{
		char* istr2 = strstr(argv[n], "-d");
		if(istr2 != NULL)
		{	
		
			If_Modified_Since = 1; //the client should include an "If-Modified-Since:" header in the HTTP request
			timeInterval = argv[n + 1]; //<time_interval> is in the format "day:hour:minute"
			
			now = time(NULL);
			now = now -(day*24*3600 + hour*3600 + min*60); //where day, hour и min are a values from input
			strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now)); //timebuf holds the correct format of the time.
			if((day < 0) && (hour < 0) && (min < 0))
			{
				printf("Time format is invalid\n");
				return 1;
			}
			//printf("If-Modified-Since: <time-interval> of %s\n", timebuf);
			break;
		}
	}
	int pic=0;
	for(n = 1; n < argc; n++)
	{
		//Search of str <URL>
		if(strncmp(argv[n], "www.", 4) == 0)	
		{	pic=4;
			b_url = argv[n];
			//printf("URL: %s\n", argv[n]);
			continue;
		}
		else if((strncmp(argv[n], "http://", 7)) == 0)
		{	pic=7;
			b_url = argv[n] + 7;
			//printf("URL: %s\n", argv[n] + 7);
			continue;
		}
		else if((strncmp(argv[n], "https://", 8)) == 0)
		{	pic=8;
			b_url = argv[n] + 8;
			//printf("URL: %s\n", argv[n] + 8);
			continue;
		}
	}
	printf("URL: %s\n",b_url,"\n");
	int position, length12, c;
	int len12=strlen(argv[argnumP]);
	int lenurl=strlen(b_url);
	char *string1=b_url;
	char *sub2=argv[argnumP];
	char sub3[20];
	char *port_num;
	position =strlen(b_url);
	port_num = (char *) malloc(sizeof(char) * 4 + 1);
	memset(port_num, 0, sizeof(port_num));
	if (strchr(string1,':')){
		for(c = 0; c<len12;	c++){
			printf("string1[c]%c\n",string1[c]);
			if (string1[c]==':'){
				position=c;
				sub2[c]='\0';
				break;
			}
			sub2[c]=string1[c];
		}
		length12=(len12-position);
		char sub[length12];
		int x;
		c=0;
		for (x=(position+1); x<(position+length12+1); x++){
			sub[c] = string1[x];
			c++;
		}
		port_num = (char *) realloc(port_num ,length12);
		strcat(port_num,sub);
		strcat(port_num,"\0");
	}

	struct hostent *server_host;
	struct sockaddr_in dest; 	
	int sockfd, bytes_read;
	char *host = b_url;
	char request[MAXBUF], message[MAXBUF]; //server_reply[MAXBUF]
	char *page;
	int temp;
    char *sends="1";
    char *recives="2";
	char *tmpchr;
	host = argv[1];
	if(argc > 2)
	{
		page = b_url;
	}
	else
	{
		page = PAGE;
	}
		int strlen1 = strlen(page);
	if(strchr(page, '/') != NULL)
	{
		char *strtmp=strdup(page);
		page = strtok(strtmp,"/");
		printf("n: %s\n",page);
		printf("page1: %s\n",page,"\n");
		printf("strlen: %d\n",strlen(page));
	}

	if(strlen(port_num)!=0){
		PORT = atoi(port_num);
	}
	// the maximum number of ports should not exceed 655536 
	if((PORT < 0)||(PORT > 65536))
	{ 
		fprintf(stderr, "Invalid port345 number %d\n",PORT);
		exit(0);
	}
	printf("port1: %d\n",PORT);
	
	/*if(port_num == NULL)
	{
		if(page == NULL)
			temp = strlen(b_url);
		else
			temp = strlen(b_url) - strlen(page);
	} 
	else 
	{
		temp = strlen(b_url) - strlen(port_num);
	}*/
			//printf("tmpchr1: %s\n",tmpchr,"\n");
	host = malloc(strlen(page) * sizeof(char));
	strncpy(host,page,strlen(page));
	printf("host1: %s\n",host);


    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1)
    {
        printf("Could not create socket");
    }
	printf("socket created\n");	
	char *IPbuffer; 
	// To retrieve host information 
	if(host != NULL)
		server_host = gethostbyname(host);
	printf("Hostname: %s\n", host); 
    IPbuffer = inet_ntoa(*((struct in_addr*)server_host->h_addr_list[0])); 
	printf("Host IP: %s\n", IPbuffer); 
	if(server_host == NULL)
	{
		printf("Can't get IP");
		exit(EXIT_FAILURE);
	}

	//Initialize server address/port
	memset(&dest,0,sizeof(dest));
	dest.sin_family = AF_INET;
  	dest.sin_port = htons(PORT);
	memcpy(&dest.sin_addr.s_addr,server_host->h_addr,server_host->h_length);

  //Connection to server
	if(connect(sockfd, (struct sockaddr*)&dest, sizeof(struct sockaddr)) < 0)
	{		
		perror("Connection failed");
		exit(1);
	}
	printf("Connection succeed\n");
	
	if(send(sockfd, request, strlen(request), 0)< 0)
	{
		perror("Can't send query\n");
		exit(1);
	}	  
	memset(request, 0, sizeof(request));
	int received = 0;
	bytes_read = read(sockfd,request,sizeof(request));
	while(bytes_read > 0) 
	{
		memset(request, 0, sizeof(request));
		bytes_read = recv(sockfd, request, MAXBUF, 0);
		if (bytes_read < 0){
	        printf("ERROR reading response from socket");
	        break;
	    }
	    if (bytes_read == 0){
    		printf("None response\n");
    	    break;
    	}
    	received+=bytes_read;
	}
	
	char *tpl = "GET /%s HTTP/1.0\r\n";
	printf("Request: %s %s %s\n",request,tpl,argv[2]);
    if(send(sockfd, request, strlen(request), 0)<0)
			perror("Can't send query\n");    	
    if(responseHeader == 0 && If_Modified_Since == 0)
		printf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nIf-Modified-Since: %s\r\nConnection: Close\r\n\r\n", page, host, timebuf);
	else if(responseHeader == 0 && If_Modified_Since == 1)
		printf(request, "GET %s HTTP/1.0\r\n Host: %s\r\nConnection: Close\r\n\r\n", page, host);
	else if(responseHeader == 1 && If_Modified_Since == 1)
		printf(request, "HEAD %s HTTP/1.0\r\n Host: %s\r\nConnection: Close\r\n\r\n", page, host);
	else
		printf(request, "HEAD %s HTTP/1.0\r\nHost: %s\r\nIf-Modified-Since: %s\r\nConnection: Close\r\n\r\n", page, host, timebuf);
	int size = strlen(request);
	int tmpres = send(sockfd, request, size, 0);
	printf("HTTP request =\n%s\nLEN = %d\n", request, size);
	request[sizeof(request)-1] = '\0';

	if(tmpres < 0)
	{
		perror("Can't send query\n");
		exit(1);
	}	  
	bytes_read = tmpres;
	int ack = recv(sockfd, request, MAXBUF, 0);
	int html_b_data = 0;
	printf("get ack\n");
	while((bytes_read = ack) > 0)
	{
		if(bytes_read < 0)
		{
			perror("Can't receive data\n");
			exit(1);
		}
		html_b_data = html_b_data + bytes_read;
		if(bytes_read < sizeof(request))
		{
			request[bytes_read] = '\0';
		}
		printf("%s", request);
	}
	printf("\n Total received response bytes: %d\n", html_b_data);
	
	//Clean up
	free(port_num);
	free(tmpchr);
	free(page);
	free(host);
	close(sockfd);

	return 0;
}
