#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    long taille_fichier;
    char nom_fichier[100];

    printf("Enter the file size (octets): ");
    scanf("%ld", &taille_fichier);


    printf("Enter the output file name: ");
    scanf("%s", nom_fichier);
    printf("...Generating.... \n");

    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) {
        printf("Error while creating file!\n");
        return 1;
    }
    for (long i = 0; i < taille_fichier; i++) {
        char c = 32 + (rand() % 95);
        fwrite(&c, sizeof(char), 1, fichier);
    }

    fclose(fichier);

    printf("File '%s' of size %ld octets generated successfully.\n", nom_fichier, taille_fichier);

    return 0;
}