#include <stdio.h>
#include <string.h>
#include "fileio.h"

int readFromFile(DATEIKARTE **anfang) {
    FILE *fptr = fopen("Karteikarten.csv", "r");
    if (fptr == NULL) return 1;
    deleteList(anfang);

    char line[FLENGTH+ALENGTH];
    char frage[FLENGTH];
    char antwort[ALENGTH];

    while (fgets(line, sizeof(line), fptr)) {
        if (sscanf(line, "%99[^;];%99[^\n]",frage, antwort) == 2) {
            addElement(anfang, frage, antwort);
        }
    }

    fclose(fptr);
    return 0;
}

void writeToFile(DATEIKARTE *anfang) {
    DATEIKARTE *current = anfang;
    FILE *fptr = fopen("Karteikarten.csv", "w");

    while (current != NULL) { // TODO: ID und pointer
        // fprintf(fptr, "%d;", current->id);
        fprintf(fptr, "%s;", current->frage);
        fprintf(fptr, "%s\n", current->antwort);
        current = current->next;
    }

    fclose(fptr);
}

void deleteFile()
{
    remove("Karteikarten.csv");
}