#include "variables.h"
#include "usuarios.h"
#include "canales.h"
#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Definimos la funcion macro, para la retroalimentacion de errores.
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

//Constantes
#define TRUE 1
#define FALSE 0

#define MAX_TAM_NICKNAME 8
#define MAX_TAM_HOSTNAME 255
#define TAM_MAX_NOMBRE_CANAL 50
#define MAX_CANALES 200
#define MAX_USUARIOS 200
#define MAX_USUARIOS_POR_CANAL 100
#define MAX_CANALES_XUSUARIO 20
#define MAX_COLA_LISTEN 5
#define TAM_BUFFER 512

//Estructura para la info del cliente.
typedef struct
{
	int sock;
	char hostname[MAX_TAM_HOSTNAME];
} InfoCliente;

//Arreglo de Usuarios
User *users[MAX_USERS] = { NULL };

//Arreglo de Canales
Channel *channels[MAX_CHANNELS] = {NULL};

// Nombre del Server
char serv_hname[MAX_HOSTNAME_LEN+1] = { '\0' };


/* Funcion que maneja las conexiones, 
al cual se le pasa la informacion del cliente. */
void * connHandler(void * cl_info)
{
	int err, ret, num_params;
	InfoCliente cl_d = *(InfoCliente *) cl_info;
	int uid;
	char buf[BUF_SIZE];
	User *user;

	/*Usamos la funcion phtread_detach para informar
	que el hilo puede ser inmediatamente utilizado
	una vez que hayamos salido de el. */
	if ( ( err = pthread_detach(pthread_self()) ) )
		{ perror2("pthread_detach", err); exit(1); }
	
	
	uid = getFirstUnusedUser();
	if (uid < 0)
	{
		printf("Too many users logged in\n");
		close(cl_d.sock);
		pthread_exit(NULL);
	}
	users[uid] = new User(cl_d.sock, cl_d.hostname, pthread_self() );
	user = users[uid];

	printf("New user created: %d (sock: %d, hostname: %s, tid: %d)\n", uid, cl_d.sock, cl_d.hostname, pthread_self());

	/* Main loop */
	do
	{
		if ( user->waitInput() < 0 )
			closeConnection(uid, cl_d.sock);
			
		if ( ( num_params = user->parseInput() ) < 0 )
			continue;
		
		pthread_mutex_lock(&user->lock);
		ret = user->act(num_params);	
		pthread_mutex_unlock(&user->lock);
			
	}while(ret != 1);

	closeConnection(uid, cl_d.sock);
}


void closeConnection(int uid, int sock)
{
	printf("Closing connection - User %d\n", uid);
	delete users[uid];
	users[uid] = NULL;
	close(sock);
	pthread_exit(NULL);
}


/* Initialize socket */
int init_socket(int port, char *hname)
{
	int sock;
	struct sockaddr_in server;

	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		{perror("socket"); return -1;}

	server.sin_family = PF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	/* Bind socket to address */
	if (bind(sock, (struct sockaddr *) &server, sizeof(server) ) < 0) 
		{perror("bind");return -1;}
	/* Listen for connections */
	if (listen(sock, MAX_LISTEN_QUEUE) < 0)
		{perror("listen");return -1;}

	gethostname(serv_hname, MAX_HOSTNAME_LEN);
	struct hostent *s_hostent = gethostbyname(serv_hname);

	return sock;
}

