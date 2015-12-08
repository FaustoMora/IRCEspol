/* 
Written by Michalis Zervos - http://michal.is
 */

#ifndef __CANAL__
#define __CANAL__

#include <string.h>

class Canal
{
	private:

	char nombre[MAX_CANALNOMBRE_LEN+1];
	int num_users;
	int canal_users[MAX_USERS_POR_CANAL];
	char tema[MAX_TEMA_LEN+1];
	int buscarUser(int id);

	public:	
	
	Canal(char * nombre);
	~Canal();
	void getNombre(char * nombre);
	int setTema(char * tema);
	int getTema(char * tema);
	int countUsers();
	int getUsers(int users[], int len);
	int getUser(int id);
	int existe(int id);
	int addUser(int id);
	int addUser(char *nombre);
	int removerUser(int id);
	int removerUser(char *name);
	void echoMensaje(int id, char * msg, int to_remitente);
	
};


extern Canal *canales[MAX_CANALES];

int validoCanal(int cid);
int getPrimerCanalSinUsar();
int getCanalPorName(char * nombre);
int canalExiste(char * nombre);

#endif