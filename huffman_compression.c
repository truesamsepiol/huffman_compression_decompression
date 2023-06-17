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

int nbTotalCaracteres = 0; //total de caractere dans le fichier
int caracteresApparus = 0; //Total de caractere reelement apparu
int nombreBitsEntree = 0; //Nombre de bit en sortie
int nombreBitsSortie = 0; //Nombre de bit en sortie

char code[] = "";
char *cle = "cleDecompression"; //chemin vers la cle de decompression
char *fichierCompresse = "fichierCompresse"; //chemin vers le fichier compresse

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

	char caractere;
	while((caractere = fgetc(fichier)) != EOF)
	{
		alphabet[caractere].occurences += 1;
		alphabet[caractere].caractere = caractere;
		if(alphabet[caractere].occurences == 1)
			caracteresApparus += 1;
		nbTotalCaracteres += 1;
	}

	nombreBitsEntree = nbTotalCaracteres * 7;
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

void generationCleDecompression()
{
	printf("\n		Debut de generation de la cle de decompression\n");
	printf("	                ......................... \n");
	FILE *cleDecompression = fopen(cle, "w");
	fprintf(cleDecompression, "%d\n", caracteresApparus);
	for(int id = 0; id < CARDINAL; id++)
	{
		int caractere = alphabet[id].caractere;
		int occurences = alphabet[id].occurences;
		if(occurences > 0)
			fprintf(cleDecompression, "%d %d\n", caractere, occurences);
	}
	printf("	                ......................... \n");
	printf("		Fin de generation de la cle de decompression\n\n");
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

//Construction du code de du fichier compressee
void constructionDuCode(caractereOccurences *noeud, char code[])
{
	//Si le fils gauche est une feuille 
	if(noeud->gauche->gauche == NULL)
	{
		int caractere = noeud->gauche->caractere;
		strcat(strcpy(alphabet[caractere].code, code), "0");
	}
	else
	{
		char codeGauche[CARDINAL];
		strcat(strcpy(codeGauche, code), "0");
		constructionDuCode(noeud->gauche, codeGauche);
	}

	//Si le fils droit est une feuille 
	if(noeud->droite->droite == NULL)
	{
		int caractere = noeud->droite->caractere;
		strcat(strcpy(alphabet[caractere].code, code), "1");
	}
	else
	{
		char codeDroite[CARDINAL];
		strcat(strcpy(codeDroite, code), "1");
		constructionDuCode(noeud->droite, codeDroite);
	}
}

void afficherCode()
{
	printf("\n			Debut Affichage des prefixes codes\n");
	printf("			         ...............\n");
	for(int car = 0; car < CARDINAL; car++)
	{
		if(alphabet[car].occurences > 0)
		{
			nombreBitsSortie += (alphabet[car].occurences * strlen(alphabet[car].code));
			printf("		Code Ascii ==> %d | frequences ==> %d | code ==> %s\n", alphabet[car].caractere, alphabet[car].occurences, alphabet[car].code);
		}
	}
	printf("			         ...............\n");
	printf("			Fin Affichage des prefixes codes\n");
}

void compression(char *cheminVersFichier)
{
	char caractereLu;
    	FILE *fichierACompressser;
    	if ((fichierACompressser = fopen(cheminVersFichier,"r")) == NULL){
      		fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n",cheminVersFichier);
 	     	exit(1);
    	}
	char *texte = malloc(sizeof(char) * nbTotalCaracteres);
	unsigned int car = 0;
    	while ((caractereLu = fgetc(fichierACompressser)) != EOF)
		texte[car++] = caractereLu;

    	fclose(fichierACompressser);

	FILE *fichierCompress = fopen(fichierCompresse, "w");

	for(car = 0; car < nbTotalCaracteres; car++)
	{
		int caractere = texte[car];
		fprintf(fichierCompress, "%s", alphabet[caractere].code);
	}

	fclose(fichierCompress);		
}

int main(int nbArg, char *arguments[])
{
	if(nbArg != 2)
	{
		printf("\nUsage : \n	%s chemin/vers/fichier/a/compresser\n\n", arguments[0]);
		exit(0);
	}

	char chaine[CARDINAL];

	initialisation();
	compteOccurences(arguments[1]);
	generationCleDecompression();
	huffman();

	printf("\n			Debut construction des prefices codes\n");
	printf("			         ...............\n");
	constructionDuCode(racine, "");
	printf("			         ...............\n");
	printf("			Fin construction des prefices codes\n");
	

	afficherCode();
	compression(arguments[1]);

	float economie = 100 - (100.*nombreBitsSortie/nombreBitsEntree);
	printf("\n\n		Compression terminee avec SUCCESS\n");
	printf("			Entree %d bits\n", nombreBitsEntree);	
	printf("			Sortie %d bits\n", nombreBitsSortie);
	printf("			Economie d'environ %d%%\n", (int)economie);
	printf("\n		Chemin vers la cle de decompression \"%s\"\n", cle);
	printf("		Chemin vers le fichier compresse    \"%s\"\n\n", fichierCompresse);
	return 0;
}
