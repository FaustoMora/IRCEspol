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


/* 
	Funcion que maneja las conexiones, 
	al cual se le pasa la informacion del cliente. 
*/
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

	//Obtenemos el primer usuario libre en el array de usuarios.
	uid = getFirstUnusedUser();

	//Validamos que no este lleno el arreglo.
	if (uid < 0)
	{
		printf("El numero de usuarios conectados, se ha excedido.\n");
		close(cl_d.sock);	//Cerramos el socket.
		pthread_exit(NULL);	//Salimos del trhead.
	}

	//Creamos el usuario.
	users[uid] = new User(cl_d.sock, cl_d.hostname, pthread_self() );
	user = users[uid];

	//Retroalimentacion del lado del servidor, acerca del nuevo usuario creado.
	printf("Usuario: %s conectado en el socket: %d, ha sido creado correctamente.\n", cl_d.hostname, cl_d.sock);

	// Loop principal para esperar el ingreso de parametros.
	do
	{
		if ( user->waitInput() < 0 )
			cerrarConexion(uid, cl_d.sock);
			
		if ( ( num_params = user->parseInput() ) < 0 )
			continue;
		
		pthread_mutex_lock(&user->lock);
		ret = user->act(num_params);	
		pthread_mutex_unlock(&user->lock);
			
	}while(ret != 1);

	cerrarConexion(uid, cl_d.sock);
}

/* 	
	Funcion que termina una conexion,
	borra el usuario de la lista actual 
	cierra el socket y sale del hilo.
*/

void cerrarConexion(int uid, int sock)
{
	printf("Closing connection - User %d\n", uid);
	delete users[uid];
	users[uid] = NULL;
	close(sock);
	pthread_exit(NULL);
}


/* 	
	Funcion que inicializa el socket
	para el usuario que actualmente se conecta.
*/
int init_socket(int port, char *hname)
{
	int sock;
	struct sockaddr_in server;

	//Creamos el Socket
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		{perror("socket"); return -1;}

	server.sin_family = PF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// Hacemos bind del socket a la direccion del servidor.
	if (bind(sock, (struct sockaddr *) &server, sizeof(server) ) < 0) 
		{perror("bind");return -1;}
	// Escuchamos conexiones por este socket.
	if (listen(sock, MAX_COLA_LISTEN) < 0)
		{perror("listen");return -1;}

	gethostname(serv_hname, MAX_HOSTNAME_LEN);
	struct hostent *s_hostent = gethostbyname(serv_hname);

	return sock;
}

