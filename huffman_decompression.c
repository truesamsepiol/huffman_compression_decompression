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

int nbOccurence = 0;

char *texte;

FILE *sortie;

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
 * lire le contenu de la cle de decompression et inscrire dans l'alphabet
*/
void lireCleDecompression(char *cheminVersFichier){
	char caractereLu;
    	FILE *fichierADecompresser;
    	if ((fichierADecompresser = fopen(cheminVersFichier,"r")) == NULL){
      		fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n",cheminVersFichier);
      		exit(1);
    	}

	int nbLignes;
	fscanf(fichierADecompresser, "%d\n", &nbLignes);
	nbOccurence = nbLignes;
	for(int ligne = 0; ligne < nbLignes; ligne++)
	{
		char caractere;
		int occurence;

		fscanf(fichierADecompresser, "%c %d\n", &caractere, &occurence);
		alphabet[ligne].caractere = caractere;
		alphabet[ligne].occurence = occurence;

	}
    	fclose(fichierADecompresser);
}

/*
 3- 7 Fonction pour creer l'arbre

    :param -> un tableau (dans notre cas ce sera le tableau deja trie des caracteres et leur occurence)
    :return -> un autre tableau qui contient les racines de notre arbre - suivant le principe de huffman 
*/

int trouveFeuille(int feuilleTemp, int racineTemp, char caractere)
{
	if(caractere == '0')
	{
		if(alphabet[feuilleTemp].occurence < noeudParent[racineTemp - 1])
			return 1;
	}
	else
	{
		if(alphabet[feuilleTemp].occurence > noeudParent[racineTemp - 1])
			return 1;
	}
	return 0;
}

void compareFeuille(int feuilleTemp, char caractere)
{	
	if(caractere == '0')
		fprintf(sortie, "%c", alphabet[feuilleTemp - 1].caractere);
	else
		fprintf(sortie, "%c", alphabet[feuilleTemp].caractere);
}

void construireArbre(char *cheminVersFichierCompresse){
   	int feuille = 0, racine = 0, k; 

    	printf("\nDebut de la construction de l'arbre\n\n");
    	printf("             .........             \n\n");

    	if(TAILLE_TABLEAU - feuille <= 1 )
       		racine = feuille;
    	else
	{
        	noeudParent[racine] = alphabet[feuille].occurence + alphabet[feuille + 1].occurence;
        	racine++;
        	for(k = feuille+2; k < nbOccurence; k++)
		{
            		noeudParent[racine] = alphabet[k].occurence + noeudParent[racine -1];
            		racine++;
        	}
    	}
    	printf("\nFin de la construction de l'arbre\n\n");

    	racine--;
    	feuille = --k;

	FILE *fichierCompresse = fopen(cheminVersFichierCompresse, "r");
	char bit;
	int racineTemp = racine;
	int feuilleTemp = feuille;
	while((bit = fgetc(fichierCompresse)) != EOF)
	{
		int gauche, droite;
		gauche = feuilleTemp;
		droite = racineTemp;

		if(racineTemp == 0)
		{
			compareFeuille(feuilleTemp, bit);
			racineTemp = racine;
			feuilleTemp = feuille;
		}
		else if(trouveFeuille(gauche, droite, bit))
		{
			fprintf(sortie, "%c", alphabet[feuilleTemp].caractere);
			racineTemp = racine;
			feuilleTemp = feuille;
		}
		else
		{
			racineTemp--;
			feuilleTemp--;
		}
	}
	printf("\n");
	fclose(fichierCompresse);

}


int main(int nbArgument, char *parametres[])
{
    	if(nbArgument != 3){
        	printf("Usage: %s chemin/vers/fichier/compresse chemin/vers/cle/decompression \n\n", parametres[0]);
        	exit(1);
    	}
	sortie = fopen("sortie", "w");
	initialisationTableauAlphabet();
    	lireCleDecompression(parametres[2]);
    	construireArbre(parametres[1]);    
	fclose(sortie);
    	return 0;
}
