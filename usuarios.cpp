/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/
 
#include "usuarios.h"

/* Clase Usuario */

/* ----------------- Constructores ----------------------*/

//Constructor
Usuario::Usuario(int sockdscr, char * nombre_host, pthread_t threadid){
	if ( (buf =(char *) malloc(TAM_BUFFER)) < 0){
		return;
    }

	this->sock = sockdscr;
	strcpy(this->nombre_host, nombre_host);
	this->tid = threadid;

	this->buf[0] = this->nickname[0] = this->nombre_usuario[0] = this->nombre_real[0] = '\0';
	this->estado = NONE;
	this->num_canales = 0;
	pthread_mutex_init(&this->lock, NULL);


}

//Destructor
Usuario::~Usuario(){
	pthread_mutex_unlock(&this->lock);
	pthread_mutex_destroy(&this->lock);
	free(buf);
}


//Método Get para obtener el nickname del usuario.
int Usuario::getNickname(char *nick){
	if (nick == NULL){
		return -1;
    }
	strcpy(nick, this->nickname);
	return 0;
}

//Método Set para definir un nickname de usuario.
int Usuario::setNickname(char *nick){
	strcpy(this->nickname, nick);
}

//Método para obtener el Nombre del usuario.
int Usuario::getNombreUsuario(char *nombre_usu){
	if (nombre_usu == NULL){
		return -1;
    }
	strcpy(nombre_usu, this->nombre_usuario);
	return 0;
}

//Método Set para definir el nombre del usuario
int Usuario::setNombreUsuario(char *nombre_usu){
	strcpy(this->nombre_usuario, nombre_usu);
}

//Método para obtener el Nombre real del usuario.
int Usuario::getNombreReal(char *nombre_r){
	if (nombre_r == NULL){
		return -1;
    }
	strcpy(nombre_r, this->nombre_real);
	return 0;
}


//Método Set para definir el nombre real del Usuario
int Usuario::setNombreReal(char *nombre_r){
	strcpy(this->nombre_real, nombre_r);
}

//Método Get para obtener el nombre del Host
int Usuario::getNombreHost(char *nombre_h){
	if (nombre_h == NULL){
		return -1;
    }
	strcpy(nombre_h, this->nombre_host);
	return 0;
}

//Función para obtener el número actual de canales
int Usuario::numCanales(){
	return this->num_canales;
}

//Método get para obtener el estado del usuario
Regstatus Usuario::getEstado(){
	return this->estado;
}

//Método Set para obtener el estado del Usuario.
void Usuario::setEstado(Regstatus rs){
	this->estado = rs;
}


//Retorna el numero del socket de la clase usuario
int Usuario::getSock(){
	return this->sock;
}

//Retorna el ID del thread de la clase usuario
pthread_t Usuario::getThreadId(){
	return this->tid;
}

//Funcion que añade a un usuario a un canal con el nombre del canal.
int Usuario::unirse(char *chan){
	return this->unirse(obtenerCanalxNombre(chan));
}

//Funcion que añade a un usuario a un canal con el id del canal.
int Usuario::unirse(int id){
	if (id < 0){
		return -1;
    }
	if (this->numCanales() >= MAX_CANALES_X_USUARIO){
		return -2;
    }
	if (this->isIn(id)){
		return -3;
    }
    
	this->canales_usuario[this->num_canales++] = id;
	canales[id]->anadirUsuario(this->nickname);
	return 1;
}

//Funcion que saca a un usuario de un canal con el nombre del canal.
int Usuario::salirse(char *chan){
	return this->salirse(obtenerCanalxNombre(chan));
}

//Funcion que saca a un usuario de un canal con el id del canal.
int Usuario::salirse(int id){
	int canal_pos;

	if (id < 0){
		return -1;
    }
	if (this->numCanales() <= 0){
		return -2;
    }
	canal_pos = this->buscarCanal(id);
	if (canal_pos < 0){
		return -3;
    }
	for (int i = canal_pos; i < this->numCanales()-1; i++){
		this->canales_usuario[i] = this->canales_usuario[i+1];
    }
	canales[id]->removerUsuario(this->nickname);

	return canales[id]->contarUsuarios();

}

//Funcion que busca un canal en el arreglo del canal de usuario por el id y lo retorna.
int Usuario::buscarCanal(int id){
	for (int i = 0; i < this->numCanales(); i++){
		if ( this->canales_usuario[i] == id ){
			return i;
        }
    }
	return -1;
}

//Funcion que valida si un usuario esta en el canal.
int Usuario::isIn(int id){
	return (this->buscarCanal(id) >= 0);
}

//Funcion que envia (write) mensaje a traves del socket
int Usuario::enviarMensaje(char * msg = NULL){
	int b_wr;

	if ( msg == NULL ){
		b_wr = write(this->sock, this->buf, strlen(this->buf));
		if (b_wr != strlen(this->buf)){
			return -1;
        }
        
	}else{	
		if (strlen(msg) >= TAM_BUFFER){
			return -1;
        }
		b_wr = write(this->sock, msg, strlen(msg));
		if (b_wr != strlen(msg)){
			return -1;
        }
	}

	return b_wr;
	
}

//Funcion que maneja la entrada de parametros.
int Usuario::esperarEntrada(){
	int lectura_incompleta = FALSE;
	int i = 0, b_read;
	char chr;

	do{
		//Lee un caracter a la vez.
		b_read = read(this->sock, &chr, 1);
		

		//Si hay un error
		if (b_read == -1){
			lectura_incompleta = TRUE;
			this->buf[i] = '\0';
			break;
		}
		//Si aplasta enter el usuario añade retorno y EOF.
		if ( chr == '\r' ){
			if ( read(this->sock, &chr, 1) > 0 ){
				if (chr == '\n'){
                    
					this->buf[i] = '\0';
					this->buf[i+1] = '\0';						
					break;
                    
				}
			}else{
				return -1;
            }
		}
		//Guarda el caracter y actualiza buf.
		this->buf[i++] = chr;

	/*Se repite el loop hasta que CR-LF es leido o 
	el mensaje se vuelve demasiado largo*/
	}while(chr != '\0' && i < TAM_BUFFER-1);

	if (lectura_incompleta){
		return -1;
    }else{
		return i;
    }
}

//Parsea los parametros que hemos ingresado por el buffer.
int Usuario::parsearEntrada(){
	
	int i = 0;
	char *tmp_buf = this->buf;	

	if ( tmp_buf == NULL ){
		return -1;
    }

	if ( *tmp_buf == ':' ){
		tmp_buf = strchr(tmp_buf, ' ') + 1;
    }

	do{
		this->cmd_parametros[i] = tmp_buf;
        
		if (*tmp_buf == ':'){
			this->cmd_parametros[i++] = tmp_buf+1;
			break;
		}

		if ( ( tmp_buf = strchr(tmp_buf, ' ') ) != NULL ){
			*tmp_buf++ = '\0';
        }
        
		i++;
	}while( tmp_buf != NULL );

	this->cmd_parametros[i] = NULL;

	return i-1;
}

//Funcion que retorna si el usuario esta registrado.
int Usuario::estaRegistrado(){
	return (this->estado == REG);
}

//Funcion que toma los valores de las constantes de los errores definidos, construye y envia el error.
void Usuario::enviarError(int err_num, char * par, char * msg){
	char *parametros[4];
	char cmd[20] = { '\0' };
	char buffer[TAM_BUFFER];

	if ( this->estaRegistrado() ){
		parametros[0] = this->nickname;
    }else{
		parametros[0] = "*";
    }
	
	parametros[1] = par;
	parametros[2] = msg;
	parametros[3] = NULL;
	sprintf(cmd,"%d\0", err_num);	

	construirMensaje(NULL, NULL, nombre_servidor, cmd, parametros, buffer);

	this->enviarMensaje(buffer);

	
}

// Funcion que recibe un mensaje y lo imprime en la pantalla del cliente
void Usuario::enviarInfo(char * par, char * msg){
	char *parametros[4];
	char cmd[20] = { '\0' };
	char buffer[TAM_BUFFER];

	parametros[0] = this->nickname;
	parametros[1] = par;
	parametros[2] = msg;
	parametros[3] = NULL;	

	construirMensaje(NULL, NULL, nombre_servidor, cmd, parametros, buffer);

	this->enviarMensaje(buffer);
}

/*Funcion principal de manejo de parametros
de entrada del cliente
*/
int Usuario::act(int num_parametros){
	char *cmd = this->cmd_parametros[0];
	char buffer[TAM_BUFFER];
	char *prms[MAX_PARAMETROS];

	if ( cmd == NULL ){
		return -1;
	}

	if ( ! strcmp(cmd, "NICK") ){
		if (num_parametros < 1 ){
			this->enviarError( INGRESANICKNAME, "" ,"No has ingresado un nickname" );	
			return -1;
		}

		if ( ! nombreUsuarioValido(this->cmd_parametros[1]) ){	
			this->enviarError( ERR_ERRONEUSNICKNAME, this->cmd_parametros[1] ,"Nickname no valido" );
			return -1;
		}

		if ( nicknameOcupado(this->cmd_parametros[1]) ){	
			this->enviarError( NICKNAMEENUSO, this->cmd_parametros[1] , "Nickname ocupado" );
			return -1;
		}

		this->setNickname(this->cmd_parametros[1]);

		switch (this->getEstado()){
		case REG:
			break;
		case NICK:
			break;
		case USER:
			this->setEstado(REG);
			this->enviarMotd();
			break;
		case NONE:
			this->setEstado(NICK);
			break;
		default:
			break;
		}

	}else if ( ! strcmp(cmd, "USER") ){

		if ( num_parametros < 4 ){
			this->enviarError( FALTANPARAMETROS, cmd ,"Faltan parametros" );
			return -1;
		}

		switch (this->getEstado()){
		case REG: case USER:
			this->enviarError( YAESTAREGISTRADO, "" ,"Ya se encuentra registrado" );
			return -1;
			break;
		case NICK:
			this->setEstado(REG);
			this->enviarMotd();
			break;
		case NONE:
			this->setEstado(USER);
			break;
		default:
			break;
		}

		this->setNombreUsuario(this->cmd_parametros[1]);
		this->setNombreReal(this->cmd_parametros[4]);
		
	}else if ( ! strcmp(cmd, "VERSION") ){

		sprintf(buffer,"Version del Servidor IRC ESPOL --> 1.0\r\n");
		this->enviarMensaje(buffer);

	}else if ( ! strcmp(cmd, "TEST") ){
			char buffer[1024] = {NULL};

			for (int i = 0; i < this->numCanales(); i++)
			{
			   printf("%d\n", i);
			}
			this->enviarMensaje(buffer);
	}else{

		if ( !this->estaRegistrado() ){
			this->enviarError( NOESTASREGISTRADO, "" , "No estas registrado" );
			return -1;
		}

		if ( ! strcmp(cmd, "QUIT") ){
			return 1;

		}else if ( ! strcmp(cmd, "JOIN") ){
			int cid; 

			if ( num_parametros < 1 ){
				this->enviarError( FALTANPARAMETROS, cmd ,"Faltan parametros" );
				return -1;
			}

			if ( ( cid = obtenerCanalxNombre(this->cmd_parametros[1]) ) < 0 ){
				printf("Creando nuevo canal - %s\n", this->cmd_parametros[1]);
				cid = obtenerCanalSinUsar();
				
				if (cid < 0){
					printf("Sala del canal repleta\n");
					return -1;
				}
				canales[cid] = new Canal(this->cmd_parametros[1]);
			}
			if ( this->unirse(cid) < 0){
				return -1;
			}

			sprintf(buffer,":%s!%s@%s Se unio al canal :%s\r\n", this->nickname, this->nombre_usuario, this->nombre_host, this->cmd_parametros[1]);
			canales[cid]->enviarMensaje(getUsuarioxNombre(this->nickname), buffer, TRUE);

			char tema[MAX_TAM_TEMA+1];
			canales[cid]->getTema(tema);
			sprintf(buffer,":%s 332 %s %s El tema del canal es:%s\r\n", nombre_servidor, this->nickname, this->cmd_parametros[1], tema);
			this->enviarMensaje(buffer);

			char tmp[MAX_TAM_NICKNAME];

			sprintf(buffer, ":%s 353 %s En el canal %s estan los siguientes usuarios:", nombre_servidor, this->nickname, this->cmd_parametros[1]);
			for ( int i = 0; i < canales[cid]->contarUsuarios(); i++){
				usuarios[canales[cid]->getUsuario(i)]->getNickname(tmp);
				strcat(buffer, tmp);
				strcat(buffer, " ");
			}
			strcat(buffer,"\r\n");
			this->enviarMensaje(buffer);

			sprintf(buffer, ":%s 366 %s Canal %s :Fin de la lista de nombres.\r\n", nombre_servidor, this->nickname, this->cmd_parametros[1]);
			this->enviarMensaje(buffer);

		}else if ( ! strcmp(cmd, "PART") ){

			int cid, num_usuarios;

			if ( num_parametros < 1 ){
				this->enviarError( FALTANPARAMETROS, cmd ,"Faltan parametros" );
				return -1;
			}

			if ( ( cid = obtenerCanalxNombre(this->cmd_parametros[1]) ) < 0 ){
				return -1;
			}

			sprintf(buffer, ":%s!%s@%s SALIO %s\r\n", this->nickname, this->nombre_usuario, this->nombre_host, this->cmd_parametros[1]);
			canales[cid]->enviarMensaje(getUsuarioxNombre(this->nickname), buffer, TRUE);

			if ( (num_usuarios = this->salirse(cid)) < 0 ){
				return -1;
			}

			if ( num_usuarios == 0 )	{
				delete canales[cid];
				canales[cid] = NULL;
			}	

		}else if ( ! strcmp(cmd, "NAMES") ){

			char cnombre[MAX_TAM_CANAL+1];
			char unombre[MAX_TAM_USERNAME+1];
			// se verifica la entrada de los parametros
			if( ! num_parametros < 1 ){
				int cindex = obtenerCanalxNombre(this->cmd_parametros[1]);

				// se pregunta por el index del canal
				if(cindex<0){
					this->enviarError( NOEXISTECANAL, cmd ,"No existe canal" );

				}else{
					// si canal existe se busca los usuarios dentro del canal
					canales[cindex]->getNombre(cnombre);
					printf("%s\n", cnombre);
					strcat(buffer, this->cmd_parametros[1]);
					strcat(buffer, " ");

					for ( int i = 0; i < canales[cindex]->contarUsuarios(); i++){
						usuarios[canales[cindex]->getUsuario(i)]->getNickname(unombre);
						strcat(buffer, unombre);
						strcat(buffer, ",");
					}
					strcat(buffer,"\r\n");
					this->enviarMensaje(buffer);

				}
				
			}else{
				// recorremos los canales para buscar cada uno de ellos
				for ( int i = 0; i < MAX_NUM_CANALES; i++){
					if( canales[i] != NULL ){

						canales[i]->getNombre(cnombre);
						strcat(buffer, cnombre);
						strcat(buffer, " ");

						for ( int j = 0; j < canales[i]->contarUsuarios(); j++){
							usuarios[canales[i]->getUsuario(j)]->getNickname(unombre);
							strcat(buffer, unombre);
							strcat(buffer, ",");
						}

					strcat(buffer,"\r\n");
					}
				}

				// para encontrar usuarios que no tienen canal
				// recorremos usuarios
				int acum = 0;
				strcat(buffer, "*");
				strcat(buffer, " ");
				for (int i = 0; i < MAX_NUM_USUARIOS; ++i){
					if( usuarios[i] != NULL){

						for(int j=0; j < MAX_NUM_CANALES;j++ ){
							if(usuarios[i]->isIn(j)){
								acum++;
							}
						}
						if(acum==0){
							usuarios[i]->getNickname(unombre);
							strcat(buffer, unombre);
							strcat(buffer, ",");
						}
						acum=0;

					}
				}
				strcat(buffer,"\r\n");
				this->enviarMensaje(buffer);
			}
			


		}else if ( ! strcmp(cmd, "LIST") ){
			char tmp[MAX_TAM_NICKNAME];
			sprintf(buffer, " Lista de los canales disponibles en el servidor: " );
			int tmpnum = obtenerCanalSinUsar();
			for ( int i = 0; i < tmpnum; i++){
				canales[i]->getNombre(tmp);
				strcat(buffer, tmp);
				strcat(buffer, " ");
			}
			strcat(buffer,"\r\n");
			this->enviarMensaje(buffer);


		}else if ( ! strcmp(cmd, "PRIVMSG") ){

			if ( num_parametros == 0 ){	

				this->enviarError( NOEXISTERECEPTOR, cmd ,"No existe receptor" );
				return -1;

			}else if ( num_parametros == 1 ){	

				this->enviarError( INGRESEALGOPARAENVIAR, cmd ,"No hay nada para enviar" );
				return -1;
			}

			/*Al canal*/
			if ( *(this->cmd_parametros[1]) == '#' ){

				int r_cid = obtenerCanalxNombre(cmd_parametros[1]);
				if (r_cid < 0){
					this->enviarError( NOEXISTENICKNAME, cmd ,"No existe canal" );
					return -1;
				}

				if ( ! this->isIn(r_cid) ){

					this->enviarError( ERR_CANNOTSENDTOCHAN, cmd_parametros[1] ,"No se puede enviar al canal" );
					return -1;
				}
				prms[0] = this->cmd_parametros[1];
				prms[1] = this->cmd_parametros[2];
				prms[2] = NULL;
				construirMensaje(this->nickname, this->nombre_usuario, this->nombre_host, cmd, prms, buffer);
				canales[r_cid]->enviarMensaje(getUsuarioxNombre(this->nickname), buffer, FALSE);

				/* Al usuario */    
			}else{

				int r_uid = getUsuarioxNombre(cmd_parametros[1]);
				if (r_uid < 0){

					this->enviarError( NOEXISTENICKNAME, cmd ,"No existe nick" );
					return -1;
				}
				prms[0] = this->cmd_parametros[1];
				prms[1] = this->cmd_parametros[2];
				prms[2] = NULL;
				construirMensaje(this->nickname, this->nombre_usuario, this->nombre_host, cmd, prms, buffer);
				usuarios[r_uid]->enviarMensaje(buffer);

			}

		}else if ( ! strcmp(cmd, "SETNAME") ){
			/* Comando que cambia el nombre real del usuario */
			char aux[60];
			// verificamos entrada de parametros
			if ( num_parametros < 1 ){
				this->enviarError( FALTANPARAMETROS, cmd ,"Faltan parametros" );
				return -1;
			}

			// cambiamos nuevo nombre
			this->setNombreReal(this->cmd_parametros[1]);

			// presentamos mensaje
			strcpy(aux,"-->");
			strcat(aux,this->nombre_real);
			this->enviarInfo( aux ,"Nombre real actualizado" );

		}else if ( ! strcmp(cmd, "INFO") ){

			sprintf(buffer,"Version del Servidor IRC ESPOL 1.0\nProyecto de Sistemas Operativos\nCreado el 16 de Diciembre del 2015\nWilson-Andres-Fausto.\nDesarrollado en C++\nLicencia GNU GPL\r\n");
			this->enviarMensaje(buffer);

		}else if ( ! strcmp(cmd, "MOTD") ){
			/* Comando que envia el mensaje del dia */
			this->enviarMotd();

		}else if ( ! strcmp(cmd, "TIME") ){
			
			char timemsg[TAM_BUFFER];
			time_t rawtime;
			struct tm * timeinfo;
			time (&rawtime);
			timeinfo = localtime (&rawtime);
			sprintf (timemsg,"LOCAL SERVER TIME (GMT): %s", asctime(timeinfo));
			this->enviarMensaje(timemsg);
		
		}else if ( ! strcmp(cmd, "USERS") ){
			/* Comando que envia la lista de usuarios conectado a ese canal*/
			char nom[MAX_TAM_NICKNAME+1];
				for (int i = 0; i < MAX_NUM_USUARIOS; i++){
					if(usuarios[i] != NULL){
						usuarios[i]->getNombreUsuario(nom);
						printf (" %d: %s \n",i+1,nom);
					}
				}

		}else{
			this->enviarError( COMANDODESCONOCIDO, cmd ,"Comando desconocido" );
			return -1;
		}
	}

	return 0;
}

//Funcion para enviar MOTD
int Usuario::enviarMotd(){
	char msg[TAM_BUFFER];
	char *prms[3];
	

	sprintf(msg,":%s 375 %s :-%s Mensaje del Dia-\r\n", nombre_servidor, this->nickname, nombre_servidor );
	this->enviarMensaje(msg);

	sprintf(msg,":%s 372 %s :Bienvenido a nuestro IRC-server Espol\r\n", nombre_servidor, this->nickname);
	this->enviarMensaje(msg);

	sprintf(msg,":%s 376 %s :Fin del comando MOTD\r\n", nombre_servidor, this->nickname);
	this->enviarMensaje(msg);
}


/* Funciones relacionadas a Usuario */

int usuarioValido(int id){
	return (usuarios[id] != NULL);
}

int getUsuarioxNombre(char * nombre_r){
	char nick[MAX_TAM_NICKNAME+1];
	for (int i = 0; i < MAX_NUM_USUARIOS; i++){
		if (usuarioValido(i)){
            
			usuarios[i]->getNickname(nick);
			if (strcmp(nick, nombre_r) == 0){
				return i;
            }
		}
	}
	return -1;
}

int getUsuarioSinUsar(){
	for (int i = 0; i < MAX_NUM_USUARIOS; i++){
        
		if ( !usuarioValido(i)){
			return i;
        }
	}
	return -1;
}

int nicknameOcupado(char * nombre_r){
	return (getUsuarioxNombre(nombre_r) >= 0 );
}
//Retorna un entero que determina si el nombre escogido es válido en el IRC
int nombreUsuarioValido(char * nombre_usu){
	int i,k;
	char valid_chrs[] = { '-', '[', ']', '\\', '\'', '^', '}', '{' };
	int ok = FALSE;

	for (i = 0; i < strlen(nombre_usu); i++){
		if ( ! isalnum(nombre_usu[i]) ){
			if (isspace(nombre_usu[i])){
                return FALSE;
            }

			for (k = 0; k < 8; k++){
				if (nombre_usu[i] == valid_chrs[k]){
					ok = TRUE;
					break;
				}
			}
			if ( ok == FALSE ){
				return FALSE;
            }
		}
		
		ok = TRUE;
	}

	return ok;
}

//Arma un mensaje utilizando varios parametros como el nickname del usuario, el comando y el texto a enviar

void construirMensaje(char *nick, char *nombre_usu, char *nombre_h, char *cmd, char *parametros[MAX_PARAMETROS], char *buf){
	int i = 0;

	memset(buf, 0, TAM_BUFFER);

	if (nombre_usu != NULL && nick != NULL){
        
		sprintf(buf, ":%s!%s@%s %s", nick, nombre_usu, nombre_h, cmd);
    }else{
        
		sprintf(buf, ":%s %s", nombre_h, cmd);
    }

	while (parametros[i] != NULL){
        
		strcat(buf, " ");
		if (parametros[i+1] == NULL){
			strcat(buf, ":");
        }

		strcat(buf, parametros[i++]);
	}
	
	strcat(buf, "\r\n");

}
