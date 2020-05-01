#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_sockets.h"
#include "game_login.h"
#include "game_files.h"
#include "common.h"

int main(int argc, char *argv[])
{
	if( argc != 2)
	{
		printf("erreur il n y a pas assez d'argument, vous devez entrez le port\n");	
		
		exit(-1);
	}
	
	int port = atoi( argv[1] );
    printf("WELCOME TO SERVER\n");

	// allocation pour chaque pseudo 
	int i, j;
	
	allocation_tab_pseudo( tab_pseudo, tab_motDepasse);
	
	/*-----------------------------------------------*/
    
	FILE *mon_flux = ouvrireFichier("test.txt");
	// recuperer les joueurs de fichier
 	recupereJoueurs( mon_flux, tab_pseudo, tab_motDepasse, &nbJoueurActuelle);
 	fclose(mon_flux);


	

	struct Client tab_client[MAX_CLIENT];
	struct Partie tab_partie[1];
	
	int nb_c = 0; // c'est le nombre le joeur actuelle en mode J ou S
	int nb_joueur = 0;;// que en mode J
	
	char buffer[BUFFSIZE] = {0};
	char newTb[BUFFSIZE] = {0};
	char quitte[BUFFSIZE] = {0};
	
	
	int selec;
	int sock = listen_socket(port);
	printf("ecoute demarre\n");
	fd_set readfds; 
	int max_fd = sock;
	int rec;
    
	// pour la connexion 
	
	while(1){
		
		FD_ZERO(&readfds);    			
		FD_SET(STDIN_FILENO, &readfds); 
		FD_SET(sock, &readfds); 
		for(i=0; i<nb_c; i++) FD_SET(tab_client[i].csock, &readfds); 
		
		
		selec = select( max_fd + 1, &readfds, NULL, NULL, NULL); 
		//printf("nb_c 2 : %d\n", nb_c);
	
		if (selec == -1)
		{
			printf("erreur de select\n");
			exit(-1);
		}
		
		// ajoue dans le readfds
		
		
	
		if(FD_ISSET(sock , &readfds))
		{
			if(nb_c < MAX_CLIENT){
			
				struct Client nv_client = add_client(sock, &nb_c, &max_fd, mon_flux);
		
							
					// juste si le client a ete ajouté le l'ajoute dans le tab_client
					if( nv_client.csock > -1 ){
				
						tab_client[nb_c - 1] = nv_client;
						
						printf("nv_client.pseudo : %s\n", nv_client.pseudo);
						// si il est en mode joueur alors 
						if( nv_client.mode == 'J'){
							
							nb_joueur++;// augmente le nombre de joueurs 
							if( nb_joueur%2 != 0){
								// je dois cree une partie
								struct Partie p ;
								p.joueur1 = nv_client;
								p.drapeau = 0;	
								tab_partie[ ((nb_joueur+1)/2)-1] = p;	
								
							
							}else{
							
								tab_partie[ (nb_joueur/2)-1].joueur2 = nv_client;
								tab_partie[ (nb_joueur/2)-1].drapeau = 1;
							
							}
						
							
						}else if(nv_client.mode == 'S'){
						
							int nb_partie = nb_joueur/2;
							//int a = send(nv_client.csock, &nb_partie, sizeof(int) , 0);

							
							for(i = 0; i < nb_joueur/2; i++){
								
								

								memset(buffer, 0, BUFFSIZE);
								int r = snprintf(buffer, BUFFSIZE, "\t%d. %s VS %s\n", i, tab_partie[i].joueur1.pseudo, tab_partie[i].joueur2.pseudo  );
																	
								send_client(nv_client.csock, buffer );
							}
								//recevoire quelle partie veut regarder
								memset(buffer, 0, BUFFSIZE);
								recv_client(nv_client.csock, buffer);
								printf("il veut voire la partie num : %s\n", buffer);
								//ici on va envoyer le board
							
						
						}
						
						printf("tab_partie[ (nb_joueur/2)-1].joueur1.pseudo : %s\n", tab_partie[ 0].joueur1.pseudo );
						printf("tab_partie[ (nb_joueur/2)-1].joueur2.pseudo : %s\n", tab_partie[ 0].joueur2.pseudo );
					}
							
				}
		}
			

		
	
		// si on a une connexion d'une des sockets qui se trouve dans le readfds
		
		
		
		for(i = 0; i < nb_joueur/2; i++)
		{
			printf("entering\n");
			if( FD_ISSET( tab_partie[i].joueur1.csock, &readfds))
			{
			
				Move coordonne;
				memset(&coordonne, -1, sizeof( coordonne) );	
						
				//printf("tab_client[i].pseudo : %s , mode : %c\n", tab_client[i].pseudo, tab_client[i].mode );
				
				memset(buffer, 0, BUFFSIZE);
				memset(newTb, 0, BUFFSIZE);
				
				

				rec = socket_recv( tab_partie[i].joueur1.csock , &coordonne , sizeof(coordonne));
                //strcat(buffer,newTb);

				if(rec==0)
				{

					// juste si on veut informer les autres de joueur est parti
					/*for(j = 0; j < nb_c; j++)
					{
						if( j != i)
						{
						
                            memset(quitte, 0, BUFFSIZE);
						    strcat(quitte, "Serveur : ");
						    strcat(quitte,tab_client[i].pseudo);
				            strcat(quitte," a quitte le jeu :(\n");
							send_client(tab_client[j].csock, quitte);
						}
					}
					*/
					rmv_client(tab_client, i, &nb_c);

					
				}else{
					

					UpdatePoint( &coordonne.from, coordonne.from.x, 7-coordonne.from.y, coordonne.from.color );
					UpdatePoint( &coordonne.to, coordonne.to.x, 7-coordonne.to.y, coordonne.to.color );

					ShowPoint(&coordonne.from);
					ShowPoint(&coordonne.to);
					socket_send( tab_partie[i].joueur2.csock , &coordonne, sizeof(coordonne));
					
					
				}
				
			}
			// pour l'autre joueur
			else if( FD_ISSET( tab_partie[i].joueur2.csock, &readfds))
			{
				Move coordonne;
				memset(&coordonne, -1, sizeof( coordonne) );	
				
				//printf("tab_client[i].pseudo : %s , mode : %c\n", tab_client[i].pseudo, tab_client[i].mode );
				
				memset(buffer, 0, BUFFSIZE);
				memset(newTb, 0, BUFFSIZE);
				
				rec = socket_recv( tab_partie[i].joueur2.csock , &coordonne , sizeof(coordonne));


				
                //strcat(buffer,newTb);

				if(rec==0)
				{

					// juste si on veut informer les autres de joueur est parti
					/*for(j = 0; j < nb_c; j++)
					{
						if( j != i)
						{
						
                            memset(quitte, 0, BUFFSIZE);
						    strcat(quitte, "Serveur : ");
						    strcat(quitte,tab_client[i].pseudo);
				            strcat(quitte," a quitte le jeu :(\n");
							send_client(tab_client[j].csock, quitte);
						}
					}
					*/
					rmv_client(tab_client, i, &nb_c);

					
				}else{
					
					UpdatePoint( &coordonne.from, coordonne.from.x, 7-coordonne.from.y, coordonne.from.color );
					UpdatePoint( &coordonne.to, coordonne.to.x, 7-coordonne.to.y, coordonne.to.color );
					socket_send( tab_partie[i].joueur1.csock , &coordonne, sizeof(coordonne));

					
					
				}
				
			}//ic

			   


		}
			
			
		
		
		//printf("nb_c : %d\n", nb_c);	
	}
	
	return 0;
}

