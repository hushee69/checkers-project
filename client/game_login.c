#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_sockets.h"
#include "game_files.h"
#include "game_login.h"

struct Client definirModeConnexion( int *nb_c, char *buffer, struct Client ret_client, FILE *flux)
{
	
	
	// envoie de premier message de bienvenu les modes à choisire
	
	send_client(ret_client.csock, "Merci de choisire un des modes de connexion suivantes:\n\n1.connexion(tapez 1)\n2.creer un compte(tapez 2)\n3.passer en mode invité(tapez 3)\n4.quitter(tapez 4)\n");	
	// recevoire la reponse de client
	recv_client( ret_client.csock, buffer);	
	//printf("buffer %d\n", atoi(buffer));
	
	switch(atoi(buffer)){
	
		case 1:
			memset(buffer, 0, strlen(buffer));
			ret_client = modeConnexion( ret_client, buffer, nb_c, flux );
			ret_client = jouerOUregarderPartie( ret_client, buffer);


			break;
			
		case 2:
			printf("\n\t\tcreation de compte\n");
			memset(buffer, 0, strlen(buffer));
			ret_client = creationCompte( ret_client, buffer, flux);
			*nb_c = *nb_c + 1;
			ret_client = jouerOUregarderPartie( ret_client, buffer);

			break;
			
		case 3:
			printf("Passage en mode invite\n");
			*nb_c = *nb_c + 1;
			
			// on donne ou joeur qui veut passer en mode invite un nombre en pseudo
			char str[20];
			memset(ret_client.pseudo, 0, TAILLE_PSEUDO);
			sprintf(str, "%d", *nb_c);
			strncpy(ret_client.pseudo, str, strlen(str));
			send_client(ret_client.csock, "\nQu'est ce que vous voulez faire?\n1.Jouer une partie(tapez 1)\n2.Regarder une partie(tapez 2)\n");
			ret_client = jouerOUregarderPartie( ret_client, buffer);


			break;
			
		case 4:
			printf("quitter\n");
			ret_client.csock = -1;
			break;
			
		default:
			printf("il y a une erreur\n");
			ret_client.csock = -1;
			break;
	}
	
	return ret_client;
	
}

struct Client modeConnexion( struct Client ret_client, char *buffer, int*nb_c, FILE *flux ){

	printf("connexion\n");
	
	// demande de pseudo et mot de passe au client 
	char *pseudo = malloc(sizeof(char)*TAILLE_PSEUDO);
	char *motDepasse = malloc(sizeof(char)*TAILLE_PSEUDO);
	
	memset(buffer, 0, strlen(buffer));
	int vrf = identificationServeur( buffer, ret_client, pseudo, motDepasse);
	
	//printf("\npseudo %s\nmotDepasse : %s\n", pseudo, motDepasse);
	
	memset(ret_client.pseudo, 0, TAILLE_PSEUDO);
	memset(ret_client.motDePasse, 0, TAILLE_PSEUDO);
	
	strncpy(ret_client.pseudo, pseudo, strlen(pseudo));
	strncpy(ret_client.motDePasse, motDepasse, strlen(motDepasse));
	
	
	// Authentification

	if( vrf == 0){
	
		// si le joueur n'est pas encore inscrit
	   //int a = send_client(ret_client.csock, "0"); 
	   printf("\nErreur : le Pseudo ou le mot de passe est faux\n");  
	   send_client(ret_client.csock, "\n0Erreur : le Pseudo ou le mot de passe est faux :(\nVoulez vous creer un compte ?\n0.NON(tapez 0 pour dire non)\n1.OUI(tapez 1 pour dire oui)\n");
	   
		   

	   
	   memset(buffer, 0, BUFFSIZE);
	   recv_client( ret_client.csock, buffer);
	   // imprimer 1 ou 0 pour dire je cree un compte
	   
	   if( (strncmp( buffer, "1", 1) == 0 )){
	   
	   		*nb_c += 1;	
	   		
	   		// le joueur veut cree un compte
	   				
	   		printf("Creation de compte \n");
	   		
	   		memset(ret_client.pseudo, 0, TAILLE_PSEUDO);
	   		memset(ret_client.motDePasse, 0, TAILLE_PSEUDO);
		
			ret_client = creationCompte( ret_client, buffer, flux);

		
			printf("ret_client pseudo : %s;;; mo de passe : %s\n", ret_client.pseudo,ret_client.motDePasse );
			//defenirMode( nb_c, buffer, ret_client);
	   		
	   }else{
	   
	   		// au revoir car il veut pas s'inscrire 
	   		printf("le joueur quit\n");
			memset(buffer, 0, BUFFSIZE);
			send_client(ret_client.csock, "Au revoir :)\n");
			ret_client.csock = -1;
	
	
	   }

	}else{


		// le joueur est deja inscrit
		*nb_c += 1;	//ici


		printf("1Connexion reussi :)\n");
		memset(buffer, 0, BUFFSIZE);
		strcat(buffer, "\nBonjour ");
		strcat(buffer, ret_client.pseudo);
		strcat(buffer, " :) Bienvenu dans le Jeu de Dame\n\nQu'est ce que vous voulez faire?\n\n1.Jouer une partie( tapez 1)\n2.Regarder une partie(tapez 2)\n");


		send_client(ret_client.csock, buffer);





   }	
	

		return ret_client;			

}

int identificationServeur( char *buffer, struct Client ret_client, char *pseudo, char *motDepasse){

	send_client(ret_client.csock, "Entrez le Pseudo : ");			
	memset(buffer, 0, BUFFSIZE);
	recv_client( ret_client.csock, buffer);

	printf("pseudo : %s\n", buffer);
	

	memset(pseudo, 0, TAILLE_PSEUDO);
	strncpy(pseudo, buffer, strlen(buffer));

	send_client(ret_client.csock, "Entrez le Mot de passe : ");
	memset(buffer, 0, BUFFSIZE);
	recv_client( ret_client.csock, buffer);
	printf("mot de passe : %s\n", buffer);
	

	memset(motDepasse, 0, TAILLE_PSEUDO);
	strncpy(motDepasse, buffer, strlen(buffer));
	
	//printf("iciiipseudo : %s\nmotDepasse : %s\n", pseudo, motDepasse);
	int vrf = verification_pseudo_mot( tab_pseudo, tab_motDepasse, pseudo, motDepasse);
	memset(buffer, 0, strlen(buffer));
	return vrf;	

}

int verification_pseudo_mot(char *tab_pseudo[], char *tab_mot_passe[], char pseudo[], char mot_passe[])
{
    int i;
    printf("dans la fonction verification_pseudo_mot\npseudo : %s, motDepasse: %s ", pseudo, mot_passe );
	for( i = 0; i< nbJoueurActuelle; i++){
		if(strncmp( tab_pseudo[i], pseudo, TAILLE_PSEUDO) == 0){

			if( strncmp( tab_motDepasse[i], mot_passe, TAILLE_PSEUDO) == 0 )
            {
                return 1;
            }
		}

	}

    return 0;
}

// je peut retourner ret_client et ejouter le mode icii
struct Client jouerOUregarderPartie( struct Client ret_client, char *buffer){

	//send_client(ret_client.csock, "");

	int a = recv_client( ret_client.csock, buffer);

	
	switch( atoi( buffer )){
	
		case 1:
			printf("il veut jouer\n");
			ret_client.mode = 'J';
			
			break;
			
		case 2:
			printf("il veut regarder une partie\n");
			ret_client.mode = 'S';
			break;
			
		default:
			printf("une erreur\n");
			//si il a tapé n'importe quoi 
			ret_client.csock = -1;
			break;
	
	}
	
	return ret_client;

}

struct Client creationCompte( struct Client ret_client, char *buffer, FILE *flux){

	memset(ret_client.pseudo, 0, TAILLE_PSEUDO);
	memset(ret_client.motDePasse, 0, TAILLE_PSEUDO);
	
	
	send_client(ret_client.csock, "Entrez le pseudo : ");
	
	memset(buffer, 0, BUFFSIZE);
	recv_client( ret_client.csock, buffer);
	printf("pseudo :%s\n", buffer);
	strcat(ret_client.pseudo, buffer);
	


	send_client(ret_client.csock, "Entrez le Mot de passe : ");
	memset(buffer, 0, BUFFSIZE);
	recv_client( ret_client.csock, buffer);
	printf("mot de passe :%s\n", buffer);
	strcat(ret_client.motDePasse, buffer);
	


    ajouteJoueur( ret_client.pseudo, ret_client.motDePasse, tab_pseudo, tab_motDepasse, &nbJoueurActuelle);
    
    
    // message de bienvenu
	memset(buffer, 0, BUFFSIZE);
	strcat(buffer, "Bienvenue dans le jeu de Dame, Voici vos idenfifiant. Pseudo : ");		    
	strcat(buffer, ret_client.pseudo);		    
	strcat(buffer, " , Mot De Passe : ");	
	strcat(buffer, ret_client.motDePasse);
	strcat(buffer, "\n\nQu'est ce que vous voulez faire?\n1.Jouer une partie( tapez 1)\n2.Regarder une partie(tapez 2)\n");			    	    
    send_client(ret_client.csock, buffer);

	return ret_client;    

}

void envoyerCouleur( struct Client ret_client, int nb_c ){
	
	char couleur = definireCouleur( nb_c );
	send_client(ret_client.csock, &couleur );
	ret_client.couleur = couleur;
}

char definireCouleur( int  nb_c ){
	
	if( nb_c%2 == 0 ){

		return 'W';
	}else{
	
		return 'B';
	}
}

// ================================
// CLIENT
// ================================
void identificationClient( int sock, char buffer[]){
	
	// Envoie de pseudo
	recv_msg(sock, buffer);
	send_msg( sock, buffer);
	
	// Envoie de mot de passe
	
	fflush(stdout);
	
	recv_msg(sock, buffer);
	send_msg( sock, buffer);
}

void definirMode( int sock, char buffer[]){

	recv_msg(sock, buffer);
	

}

void modeConnexionClient(int sock, char buffer[]){

		  printf("\n\t\t\t*************\n");
		     printf("\t\t\t*           *\n");
		  printf("\t\t\t* Connexion *\n");
		  printf("\t\t\t*           *");
		  printf("\n\t\t\t*************\n");

	identificationClient( sock, buffer);
	
	// recevoir le message de serveur si l'identification est bien passé ou pas
	recv_msg(sock, buffer);
	// si j'ai reçu 0 c'est a dire que identification n'est pas bien passé et mes identifiant ne sont pas encore dans la base de donnee

	if( buffer[1] == '0' ){

		
		// j'envoie 0 si je ne veux pas creer de compte 1 si je veux creer de compte
		send_msg( sock, buffer);	

		// je veux cree un compte			
		if(buffer[0] == '1'){
			

			identificationClient( sock, buffer);
			
			// massage de bienvenue + les 2 mode spectateur et 
			recv_msg(sock, buffer);


			
		// je veux pas cree de compte			
		}else{
	
			recv_msg(sock, buffer);
			exit(-1);
		}

	}else{

		// si le client est déja inscrie
		// je recoie le msg de bienvenu + les proposition de mode de connexion spectatteur ou joueur
		//recv_msg(sock, buffer);// pkkkk cette merd qui m'a pouri la vie pendant 2 heure fuck you
		
		// mode 
		//definirMode(sock, buffer);
	}
}

void listeParties( int sock, char buffer[]){

	//recevoir la liste des parties
	printf("La liste des parties : \n\n");
	recv_msg(sock, buffer);
	
	printf("Choisissez une partie : \n");
	//envoie de num de la partie
	send_msg( sock, buffer);
	

}

void jouerOUregarderPartieClient( int sock, char buffer[]){

	// envoyer la reponse si je veux jouer ou regarder une partie
	
	send_msg( sock, buffer);
	
	//printf("atoi(buffer) : %d\n", atoi(buffer));
	switch(atoi(buffer)){
	
		case 1:
			printf("\n\t\tvous etes en mode joueur\n\n");
			break;
			
		case 2:
		
			printf("\n\t\tVous etes en mode spectateur\n\n");
			listeParties( sock, buffer);
			
			break;
			
		default:
			printf("il y a une erreur\n");
			exit(-1);
			break;
	}
}

void definirModeConnexionClient( int sock, char buffer[]){

	printf("\n\t\t****************************\n");
	printf("\t\t*                          *\n");
	printf("\t\t*  Bienvenue sur DAME.COM  *\n");
	printf("\t\t*                          *\n");
	printf("\t\t****************************\n\n");
	// recevoire les 3 mode de connexion 
	recv_msg(sock, buffer);
	// envoyer un mode
	send_msg( sock, buffer);
	
	switch(atoi(buffer)){
	
		case 1:
			modeConnexionClient( sock, buffer);
			jouerOUregarderPartieClient( sock, buffer);


			break;
			
		case 2:
			printf("\n\t\tCreation de compte\n");
			// je veux cree un compte
			identificationClient( sock, buffer);
			// massage de bienvenue
			recv_msg(sock, buffer); 			// mode spectateur ou joueur
			jouerOUregarderPartieClient( sock, buffer);
			break;
			
		case 3:
			printf("\n\t\tPasssage en mode invite\n");
			recv_msg(sock, buffer); 			// mode spectateur ou joueur
			jouerOUregarderPartieClient( sock, buffer);
			break;
			
		case 4:
			printf("quitter\n");
			exit(-1);
			break;
			
		default:
			printf("il y a une erreur\n");
			exit(-1);
			break;
	}
}

char definirCouleurClient( int sock, char buffer[]){
	
	char couleur;
	recv(sock, &couleur, 1, 0);// recevoire le couleur B ou W
	if( couleur == 'W')
		printf("vous allez jouer avec les Blanc \n\n");
	else
		printf("vous allez jouer avec les Noir \n\n");


	return couleur;

}

