#include <stdio.h>
#include <stdlib.h>

#define CARDINAL 128 //Nous travaillons ici avec le code ASCII uniquement


typedef struct caractereOccurences
{
	int caractere;
	int occurences;
}caractereOccurences;

caractereOccurences alphabet[CARDINAL];

int caracteresApparus = 0; //Total de caractere reelement apparu

//Mettre toutes les occurences a zero
void initialisation()
{
	for(int id = 0; id < CARDINAL; id++)
		alphabet[id].occurences = 0;
}

void compteOccurences(char *cheminVersFichier)
{
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
	}

	printf("%d Caractere(s) apparus\n", caracteresApparus);
	for(int id = 0; id < CARDINAL; id++)
	{
		int caractere = alphabet[id].caractere;
		int occurences = alphabet[id].occurences;
		if(occurences > 0)
			printf("car == %c | codeAscii == %d | nombre d'occurence == %d\n", caractere, caractere, occurences);
	}
	fclose(fichier);
}


int main(int nbArg, char *arguments[])
{
	if(nbArg != 2)
	{
		printf("\nUsage : \n	%s chemin/vers/fichier/a/compresser\n\n", arguments[0]);
		exit(0);
	}

	initialisation();
	compteOccurences(arguments[1]);
	return 0;
}
