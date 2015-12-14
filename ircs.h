/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo de cabecera para canales.cpp
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/
 
#ifndef __IRCS__
#define __IRCS__

#include "constantes.h"
#include "usuarios.h"
#include "canales.h"
#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//Libreria auxiliar para manejar de nombre de host.
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Definimos una funcion macro, para la retroalimentacion de errores.
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

// Estrutuctura de datos para almacenar la info del Cliente.
typedef struct
{
	int sock;
	char hostname[MAX_TAM_HOSTNAME];
} InfoCliente;

// Variable externa del ircs. 
extern char nombre_servidor[MAX_TAM_HOSTNAME+1];

// Funciones del ircs. 
void * connHandler(void * cliente);
int init_socket(int port, char * hname);
void closeConnection(int uid, int sock);

#endif
