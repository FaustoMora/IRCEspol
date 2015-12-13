/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/

//#include "variables.h"
//#include "usuarios.h"
//#include "canales.h"
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
#include "ircs.h"
#include <iostream>
#include <unistd.h> 
#include <stdlib.h> 
#include <pthread.h>

//Definimos la funcion macro, para la retroalimentacion de errores.
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

Usuario *usuarios[MAX_NUM_USUARIOS] = { NULL };
Canal *canales[MAX_NUM_CANALES] = {NULL};
char serv_hname[MAX_TAM_HOSTNAME+1] = { '\0' };


// funcion main
int main(int argc, char *argv[]){
    
	int srv_sock, cl_sock, port;
	struct sockaddr_in client;
	struct hostent *rem;
	pthread_t tid;
	socklen_t cl_len = sizeof (struct sockaddr_in);
	int err;
	InfoCliente client_data;
	
	if ( argc != 2 ){ 
		printf("Usando: %s <port>\n", argv[0]); exit(1); 
	}

	port = atoi(argv[1]);

	if ( (srv_sock = init_socket(port, serv_hname)) < 0 ){
		exit(1);
	}
	
	
	while(1){
		while ( (cl_sock = accept(srv_sock, (struct sockaddr *) &client, &cl_len) ) < 0 );

		if ( ( rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof (client.sin_addr.s_addr), client.sin_family) ) == NULL ){ 
			perror("gethostbyaddr"); 
			exit(1); 
		}
		
		//Cliente conectado
		printf("Accepted connection from: %s\n", rem->h_name);

		client_data.sock = cl_sock;
		strcpy(client_data.hostname, rem->h_name);
		if ( ( err = pthread_create(&tid, NULL, connHandler, (void *) &client_data) ) ){ 
			perror2("phtread_create", err); 
			exit(1); 
		}
	}	
	
	return 0;
}

/* 	
	Funcion que termina una conexion,
	borra el usuario de la lista actual 
	cierra el socket y sale del hilo.
*/

void cerrarConexion(int uid, int sock){
	printf("Cerrando Conexion - Usuario ID: %d\n", uid);
	delete usuarios[uid];
	usuarios[uid] = NULL;
	close(sock);
	pthread_exit(NULL);
}


/* 
	Funcion que maneja las conexiones, 
	al cual se le pasa la informacion del cliente. 
*/
void * connHandler(void * cl_info){
	// variable User, buffer y structura de infoCliente
	int err, ret, num_params;
	InfoCliente cl_d = *(InfoCliente *) cl_info;
	int uid;
	char buf[TAM_BUFFER];
	Usuario *user;

	/*Usamos la funcion phtread_detach para informar
	que el hilo puede ser inmediatamente utilizado
	una vez que hayamos salido de el. */
	if ( ( err = pthread_detach(pthread_self()) ) ){ 
		perror2("pthread_detach", err); 
		exit(1); 
	}

	//Obtenemos el primer usuario libre en el array de usuarios.
	uid = getUsuarioSinUsar();

	//Validamos que no este lleno el arreglo.
	if (uid < 0){

		printf("El numero de usuarios conectados, se ha excedido.\n");
		close(cl_d.sock);	//Cerramos el socket.
		pthread_exit(NULL);	//Salimos del trhead.
	}

	//Creamos el usuario.
	usuarios[uid] = new Usuario(cl_d.sock, cl_d.hostname, pthread_self() );
	user = usuarios[uid];

	//Retroalimentacion del lado del servidor, acerca del nuevo usuario creado.
	printf("Usuario: %s conectado en el socket: %d, ha sido creado correctamente.\n", cl_d.hostname, cl_d.sock);

	// Loop principal para esperar el ingreso de parametros.
	do
	{
		// se espera entrada de usuario, si da es error
		if ( user->esperarEntrada() < 0 ){
			cerrarConexion(uid, cl_d.sock);
		}
		// si no recibe entrada continua	
		if ( ( num_params = user->parsearEntrada() ) < 0 ){
			continue;
		}
		
		pthread_mutex_lock(&user->lock);
		ret = user->act(num_params);	
		pthread_mutex_unlock(&user->lock);
			
	}while(ret != 1);

	cerrarConexion(uid, cl_d.sock);
}


/* 	
	Funcion que inicializa el socket
	para el usuario que actualmente se conecta.
*/
int init_socket(int port, char *hname){
	int sock;
	struct sockaddr_in server;

	//Creamos el Socket
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket"); 
		return -1;
	}
	
	// constantes predefinidas
	server.sin_family = PF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// Hacemos bind del socket a la direccion del servidor.
	if (bind(sock, (struct sockaddr *) &server, sizeof(server) ) < 0){
		perror("bind");
		return -1;
	}

	// Escuchamos conexiones por este socket.
	if (listen(sock, MAX_COLA_ESCUCHA) < 0){
		perror("listen");
		return -1;
	}
	
	gethostname(serv_hname, MAX_TAM_HOSTNAME);
	struct hostent *s_hostent = gethostbyname(serv_hname);

	return sock;
}


