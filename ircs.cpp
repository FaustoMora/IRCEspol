/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/

#include "ircs.h"

Usuario *usuarios[MAX_NUM_USUARIOS] = { NULL };
Canal *canales[MAX_NUM_CANALES] = {NULL};
char nombre_servidor[MAX_TAM_HOSTNAME+1] = { '\0' };


// Main
int main(int argc, char *argv[]){
    
	int srv_sock, cl_sock, port;
	struct sockaddr_in cliente;
	//Estructura de datos de libreria netbd.h para manejar las conexiones de los clientes.
	struct hostent *rem;
	pthread_t tid;
	socklen_t cl_len = sizeof (struct sockaddr_in);
	int err;
	InfoCliente datos_cliente;
	
	//Si no ha ingresado ningun puerto, retroalimentamos el uso de los puertos.
	if ( argc != 2 ){ 
		printf("Uso Correcto: %s <puerto>\n", argv[0]); exit(1); 
	}

	//Transformamos a caracter el puerto ingresado.
	port = atoi(argv[1]);


	//Inicializamos el socket si ha sido creado correctamente retroalimentamos, sino salimos.
	if ( (srv_sock = init_socket(port, nombre_servidor)) < 0 ){
		exit(1);
	}
	
	//Bucle infinito de escucha en el servidor.
	while(1){

		//Mientras no obtengamos ningun mensaje, seguimos esperando.
		while ( (cl_sock = accept(srv_sock, (struct sockaddr *) &cliente, &cl_len) ) < 0 );

		//Intentamos Obtener el nombre del host que se ha conectado, si obtenemos error disparamos macro de error y salimos.
		if ( ( rem = gethostbyaddr((char *) &cliente.sin_addr.s_addr, sizeof (cliente.sin_addr.s_addr), cliente.sin_family) ) == NULL ){ 
			perror("gethostbyaddr"); 
			exit(1); 
		}
		
		//Si obtenemos el nombre del cliente conectado entonces retroalimentamos conexion exitosa del cliente.
		printf("Conexion aceptada de: %s\n", rem->h_name);

		//Rellenamos la estructura de datos_cliente
		datos_cliente.sock = cl_sock;
		strcpy(datos_cliente.hostname, rem->h_name);


		//Creamos el trhead para la conexion del cliente, si algo falla retroalimentamos error y salimos.
		if ( ( err = pthread_create(&tid, NULL, connHandler, (void *) &datos_cliente) ) ){ 
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
	// Variable User, buffer y structura de infoCliente
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

	//Validamos que no este llena la lista.
	if (uid < 0){

		printf("El numero de usuarios conectados, se ha excedido.\n");
		close(cl_d.sock);	//Cerramos el socket.
		pthread_exit(NULL);	//Salimos del trhead.
	}

	//Creamos el usuario.
	usuarios[uid] = new Usuario(cl_d.sock, cl_d.hostname, pthread_self() );
	user = usuarios[uid];

	//Retroalimentacion del lado del servidor, acerca del nuevo usuario creado.
	printf("Usuario: %s con ID: %d, ha sido creado correctamente.\n", cl_d.hostname, uid);

	// Loop principal para esperar el ingreso de parametros.
	do
	{
		// Se espera entrada de usuario, si algo sale mal cierra la conexion.
		if ( user->esperarEntrada() < 0 ){
			cerrarConexion(uid, cl_d.sock);
		}
		// Si no recibe entrada continua esperando.	
		if ( ( num_params = user->parsearEntrada() ) < 0 ){
			continue;
		}
		
		//Productor - Consumidor 
		pthread_mutex_lock(&user->lock);
		ret = user->act(num_params);	
		pthread_mutex_unlock(&user->lock);
			
	}while(ret != 1); 

	//Si QUIT sale del loop y cerramos la conexion.
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
	
	// Constantes predefinidas
	server.sin_family = PF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// Hacemos bind del socket a la direccion del servidor.
	if (bind(sock, (struct sockaddr *) &server, sizeof(server) ) < 0){
		perror("bind");
		return -1;
	}
	printf("Servidor creado correctamente en el puerto: %d\n", port);
		
	// Escuchamos conexiones por este socket.
	if (listen(sock, MAX_COLA_ESCUCHA) < 0){
		perror("listen");
		return -1;
	}
	printf("Empezamos a escuchar las conexiones.\n");
	
	//Funcion de manejo de hostnames.
	gethostname(nombre_servidor, MAX_TAM_HOSTNAME);
	struct hostent *s_hostent = gethostbyname(nombre_servidor);

	return sock;
}

