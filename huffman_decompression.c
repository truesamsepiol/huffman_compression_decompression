#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CARDINAL 128 //Nous travaillons ici avec le code ASCII uniquement


typedef struct caractereOccurences
{
	int caractere;
	int occurences;
	char code[CARDINAL];
	struct caractereOccurences *gauche;
	struct caractereOccurences *droite;
	struct caractereOccurences *suivant;
}caractereOccurences;

caractereOccurences alphabet[CARDINAL];
caractereOccurences *racine;

int caracteresApparus = 0; //Total de caractere reelement apparu
char *fichierDecompresse = "fichierDecompresse"; //chemin vers le fichier decompresse

//Mettre toutes les occurences a zero
void initialisation()
{
	printf("\n		Debut Initialisation du tableau alphabet \n");
	printf("			  ....................		\n");
	for(int id = 0; id < CARDINAL; id++)
	{
		alphabet[id].caractere = 0;
		alphabet[id].occurences = 0;
        	strcpy(alphabet[id].code, "");
		alphabet[id].gauche = NULL;
		alphabet[id].droite = NULL;
		alphabet[id].suivant = NULL;
	}
	printf("			  ....................		\n");
	printf("		Fin Initialisation du tableau alphabet \n\n");
}

void initialisationFile()
{
	racine = NULL;
	for(int id = 0; id < CARDINAL; id++)
	{
		if(alphabet[id].occurences > 0)
		{
			caractereOccurences *noeud = malloc(sizeof(caractereOccurences));
			noeud->caractere = alphabet[id].caractere;
			noeud->occurences = alphabet[id].occurences;
			noeud->gauche = NULL;
			noeud->droite = NULL;
			noeud->suivant = racine;
			racine = noeud;
		}
	}
}

//Compter les frequences d'apparutions des caracteres dans un fichier
void compteOccurences(char *cheminVersFichier)
{
	printf("\n		Debut du comptage du nombre d'occurence\n");
	printf("		          ...................\n");
	FILE *fichier = fopen(cheminVersFichier, "r");
	if(fichier == NULL)
	{
		printf("\n\n		ECHEC : Impossible d'ouvir le fichier \"%s\"\n", cheminVersFichier);
		exit(1);
	}

	fscanf(fichier, "%d\n", &caracteresApparus);
	for(int ligne = 0; ligne < caracteresApparus; ligne++)	
	{
		int caractere, occurences;
		fscanf(fichier, "%d %d\n", &caractere, &occurences);
		alphabet[caractere].occurences = occurences;
		alphabet[caractere].caractere = caractere;
	}

	printf("		%d Caractere(s) apparus\n", caracteresApparus);
	for(int id = 0; id < CARDINAL; id++)
	{
		int caractere = alphabet[id].caractere;
		int occurences = alphabet[id].occurences;
		if(occurences > 0)
			printf("		car == %c | codeAscii == %d | nombre d'occurence == %d\n", caractere, caractere, occurences);
	}
	fclose(fichier);
	printf("		          ...................\n");
	printf("		Fin du comptage du nombre d'occurence\n\n");
}


caractereOccurences *extractionMinimum()
{
	if(racine->suivant == NULL)
		return racine;
	
	caractereOccurences *parcourt = racine->suivant;
	caractereOccurences *_min = racine;
	caractereOccurences *precedent = racine;
	caractereOccurences *precMin = racine;
	while(parcourt != NULL)
	{
		if(parcourt->occurences < _min->occurences)
		{
			_min = parcourt;
			precMin = precedent;
		}
		precedent = parcourt;
		parcourt = parcourt->suivant;
	}

	if(_min == racine)
		racine = _min->suivant;
	else
		precMin->suivant = _min->suivant;

	_min->suivant = NULL;
	return _min;
}

//Contruction de l'arbre
void huffman()
{
	printf("\n		Debut de contruction de l'arbre\n");
	printf("		      .................\n");

	initialisationFile();
	for(int id = 0; id < caracteresApparus - 1; id++)
	{
		caractereOccurences *noeud = malloc(sizeof(caractereOccurences));
		noeud->gauche = extractionMinimum();
		noeud->droite = extractionMinimum();
		noeud->occurences = noeud->gauche->occurences + noeud->droite->occurences;
		noeud->suivant = racine;
		racine = noeud;
	}
	printf("		      .................\n");
	printf("		Fin de contruction de l'arbre\n\n");
}

void decompression(char *cheminVersFichier)
{
	printf("		Debut decompression\n");
	printf("		  ..............\n");
	FILE *fichier = fopen(cheminVersFichier, "r");
	FILE *sortie = fopen(fichierDecompresse, "w");
	char bit;
	caractereOccurences *parcourt = racine;
	while((bit = fgetc(fichier)) != EOF)
	{
		if(bit == '0')
			parcourt = parcourt->gauche;
		else
			parcourt = parcourt->droite;

		if(parcourt->gauche == NULL)
		{
			fprintf(sortie, "%c", parcourt->caractere);
			parcourt = racine;
		}
	}
	fclose(fichier);
	fclose(sortie);
	printf("		  ..............\n");
	printf("		Fin decompression\n\n");
}

int main(int nbArg, char *arguments[])
{
	if(nbArg != 3)
	{
		printf("\nUsage : \n	%s chemin/vers/fichier/compresse chemin/ver/la/cle\n\n", arguments[0]);
		exit(0);
	}

	initialisation();
	compteOccurences(arguments[2]);
	huffman();
	decompression(arguments[1]);

	printf("		Decompression terminee avec SUCCESS\n");
	printf("		\"%s\" contient le contenu decompresse\n\n", fichierDecompresse);
	return 0;
}
