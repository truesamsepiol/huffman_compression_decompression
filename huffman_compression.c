#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE_TABLEAU 95   //pour ne traiter que les caracteres imprimable du code ascii
#define PREMIER_CARACTERE 32    

#define FICHIEROCCURENCE "caractereOccurence.txt"

typedef struct caractereOccurence{
	char caractere;
	int occurence;
	char code[TAILLE_TABLEAU];
}caractereOccurence;

/*
- Le tableau alphabet contient les caracteres ascii 32-126 d'ou la taille 95 (127-32)
- Il contient les caracteres ascii possibles et leur donne d'occurence
*/
caractereOccurence alphabet[TAILLE_TABLEAU]; 

/*
- Le tableau noeudParent contient les racines obtenus a partir de la somme des occurences selon le principe de huffman
*/
int noeudParent[TAILLE_TABLEAU]; 

int nbTotalCaractere = 0;

int nbCaractereUnique = 0;

char *texte;

/*
cette fonction permet d'initialiser le tableau de l'alphabet
- Lors de l'initialisation, les caracteres de chaque case correspondent aux indices du tableau et les occurences sont mis a 0 pour tout le monde
Tout ceci pour nous faciliter la manipulation. 
*/
void initialisationTableauAlphabet(){
	for(int indice = 0; indice < TAILLE_TABLEAU; indice++)
	{	
        	alphabet[indice].caractere = indice;
        	alphabet[indice].occurence = 0;
        	strcpy(alphabet[indice].code, "");
    	}
}


/*
Cette fonction  compare deux éléments > Elle va etre utilise dans qsort qui est une fonction deja definie en C et a une structure bien definie
- Elle prend en parametre deux pointeurs sur des constantes de n'importe quel type d'ou le void; et renvoie un entier
- Dans notre cas on fait en sorte qu'elle prenne des pointeurs sur le type qu'on a defini
*/
int fonctionDeComparaison (const void *parametre1, const void *parametre2){

    	int nombre1 = ((caractereOccurence *)parametre1)->occurence;
	int nombre2 = ((caractereOccurence *)parametre2)->occurence;

    	return (nombre1 - nombre2);
}

/* 
1 & 2 - Cette fonction  compte le nombre d'occurence des caracteres de notre fichier
    :param -> le chemin vers le fichier (le fichier a compresser)
    :return -> un tableau (de type enregistrement) qui contient chaque caractere contenus dans le fichier avec son nombre d'occurence chacun
*/
void compterOccurenceMot(char *cheminVersFichier){
	char caractereLu;
    	FILE *fichierACompressser;
    	if ((fichierACompressser = fopen(cheminVersFichier,"r")) == NULL){
      		fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n",cheminVersFichier);
      		exit(1);
    	}
    	while ((caractereLu = fgetc(fichierACompressser)) != EOF){
        	int indice = caractereLu - PREMIER_CARACTERE; //indice represente l'indice du caractere lu dans le tableau
        	if(indice >= 0 && indice < TAILLE_TABLEAU)
		{
			if(alphabet[indice].occurence == 0)
				nbCaractereUnique++;

            		alphabet[indice].occurence++;         
			nbTotalCaractere++;
		}
    	}
    	fclose(fichierACompressser);

    	// La fonction de la bibliothèque C void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*)) trie un tableau.
    	/*Elle va donc nous permettre de trier notre tableau de caractere dans l'ordre croissant du nombre d'occurence*/
    	qsort(alphabet, TAILLE_TABLEAU, sizeof(caractereOccurence), fonctionDeComparaison);
}


/*
 3- 7 Fonction pour creer l'arbre

    :param -> un tableau (dans notre cas ce sera le tableau deja trie des caracteres et leur occurence)
    :return -> un autre tableau qui contient les racines de notre arbre - suivant le principe de huffman 
*/

void construireArbre(){
   	int feuille = 0, racine = 0, k; 

    	printf("\nDebut de la construction de l'arbre\n\n");
    	printf("             .........             \n\n");
    	while(feuille < TAILLE_TABLEAU && alphabet[feuille].occurence == 0)
        	feuille++;

    	if(TAILLE_TABLEAU - feuille <= 1 )
       		racine = feuille;
    	else
	{
        	noeudParent[racine] = alphabet[feuille].occurence + alphabet[feuille + 1].occurence;
        	racine++;
        	for(k = feuille+2; k < TAILLE_TABLEAU; k++)
		{
            		noeudParent[racine] = alphabet[k].occurence + noeudParent[racine -1];
            		racine++;
        	}
    	}
    	printf("\nFin de la construction de l'arbre\n\n");

    	racine--;
    	feuille = --k;
    	printf("\nDebut de construction des codes\n\n");
    	char chaine[TAILLE_TABLEAU] = ""; //on se met au niveau de la racine pour commencer le parcours
    	while(racine >= 1)
	{
        	strcpy(alphabet[feuille].code, chaine);
        	if(noeudParent[racine - 1] > alphabet[feuille].occurence)
		{
            		strcat(alphabet[feuille].code, "0");
            		strcat(chaine, "1"); //chaine = chaine + 1
        	}
		else
		{
            		strcat(alphabet[feuille].code, "1");
            		strcat(chaine, "0");
        	}
        	feuille--;
        	racine--; 
    	}

    	strcat(strcpy(alphabet[feuille].code, chaine), "1");
    	strcat(strcpy(alphabet[feuille - 1].code, chaine), "0");

    	for(int indice = 0; indice < TAILLE_TABLEAU; indice++)
       		if(alphabet[indice].occurence != 0)
           		printf("id == %d, caractere == %c , nboccurence == %d, code == %s\n", indice, alphabet[indice].caractere + PREMIER_CARACTERE, alphabet[indice].occurence, alphabet[indice].code);
    	printf("\nFin de construction des codes\n\n");
}

/* Ecrire les frequences dans un fichier */
void enregistrerFrequence()
{
	FILE *fichierArbre = fopen("cleDecompression", "w");
	fprintf(fichierArbre, "%d\n", nbCaractereUnique);
	for(int indice = 0; indice < TAILLE_TABLEAU; indice++)
		if(alphabet[indice].occurence != 0)
           		fprintf(fichierArbre, "%c %d\n", alphabet[indice].caractere + PREMIER_CARACTERE, alphabet[indice].occurence);	
	fclose(fichierArbre);
}

/* Sortir le fichier compresse */
void enregistrerFichierCompresser(char *cheminVersFichier)
{
	char caractereLu;
    	FILE *fichierACompressser;
    	if ((fichierACompressser = fopen(cheminVersFichier,"r")) == NULL){
      		fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n",cheminVersFichier);
 	     	exit(1);
    	}

	texte = malloc(sizeof(char) * nbTotalCaractere);
	char car = 0;
    	while ((caractereLu = fgetc(fichierACompressser)) != EOF){
        	int indice = caractereLu - PREMIER_CARACTERE; //indice represente l'indice du caractere lu dans le tableau
        	if(indice >= 0 && indice < TAILLE_TABLEAU)
			texte[car++] = caractereLu;

    	}
	texte[car] = '\0';
    	fclose(fichierACompressser);

	FILE *fichierCompresse = fopen("fichierCompresse", "w");

	for(car = 0; car < nbTotalCaractere; car++)
	{
		int indice = texte[car] - PREMIER_CARACTERE;
		int id = TAILLE_TABLEAU;
		while(alphabet[id].caractere != indice)
			id--;
		fprintf(fichierCompresse, "%s", alphabet[id].code);
	}

	fclose(fichierCompresse);
	free(texte);
	printf("\n		Compression terminee avec SUCESS		\n");
	printf("\n\nLes fichier suivant ont ete genere:\n");
	printf("		1- fichierCompresse : contient le texte compresse\n");
	printf("		2- cleDecompression : la cle qui va permettre de decompresser\n\n");
}


int main(int nbArgument, char *parametres[]){
    	if(nbArgument != 2){
        	printf("Usage: %s fichier\n\n", parametres[0]);
        	exit(1);
    	}

    	initialisationTableauAlphabet();
    	compterOccurenceMot(parametres[1]);
    	construireArbre();    
	enregistrerFrequence();
	enregistrerFichierCompresser(parametres[1]);

    	return 0;
}
