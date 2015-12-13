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
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

// estrutuctura de dato para sostener el info del Cliente
typedef struct
{
	int sock;
	char hostname[MAX_TAM_HOSTNAME];
} InfoCliente;

// funciones del ircs 
extern char serv_hname[MAX_TAM_HOSTNAME+1];

void * connHandler(void * client);
int init_socket(int port, char * hname);
void closeConnection(int uid, int sock);

#endif
