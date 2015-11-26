#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//Si algo sale mal, salimos.
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//Main.
int main(int argc, char *argv[])
{
	//Variables
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// Si no pasamos el puerto como parametro, mostramos mensaje y salimos.
	if (argc < 2) {
	 fprintf(stderr,"ERROR, no port provided\n");
	 exit(1);
	}

	//Creamos el socket.
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//Si no se puede crear el socket mostramos error.
	if (sockfd < 0) 
	error("ERROR opening socket");
	
	//Igual que memset(), bzero deprecated.
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	//Obtenemos el puerto recibimos el parametro de entrada y lo convertimos a int.
	portno = atoi(argv[1]);

	//Configuramos el servidor.
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//Hacemos binding con la estructura del sockadrr a la estructura serv_addr.
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0) 
	      error("ERROR on binding");

	listen(sockfd,5);
	//Ola k ase
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, 
		 (struct sockaddr *) &cli_addr, 
		 &clilen);

	if (newsockfd < 0) 
	  error("ERROR on accept");

	bzero(buffer,256);
	n = read(newsockfd,buffer,255);

	if (n < 0) error("ERROR reading from socket");
	printf("Here is the message: %s\n",buffer);
	n = write(newsockfd,"I got your message",18);
	if (n < 0) error("ERROR writing to socket");

	close(newsockfd);
	close(sockfd);

	return 0; 
}
