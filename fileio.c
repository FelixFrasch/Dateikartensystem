#include <stdio.h>
#include <string.h>
#include "fileio.h"

int readFromFile(DATEIKARTE **anfang) {
    FILE *fptr = fopen("Karteikarten.csv", "r"); // öffnet die Datei zum Lesen ("r" = read), gibt NULL zurück, falls Datei nicht existiert
    if (fptr == NULL) return 1;
    deleteList(anfang); // alte Liste löschen bevor neue Daten geladen werden

    char line[FLENGTH + ALENGTH + 2];
    char frage[FLENGTH];
    char antwort[ALENGTH];

    char format[50];
    sprintf(format, "%%%d[^;];%%%d[^\n]", FLENGTH - 1, ALENGTH - 1); // Baut z.B. "%99[^;];%99[^\n]" dynamisch aus FLENGTH/ALENGTH

    while (fgets(line, sizeof(line), fptr)) { // liest eine Zeile aus der Datei in line
        if (sscanf(line, format, frage, antwort) == 2) { // parst "frage;antwort" aus line, == 2 prüft, ob beide Felder gelesen wurden
            addElement(anfang, frage, antwort);
        }
    }

    fclose(fptr); // Datei schließen, gibt den Datei-Handle frei
    return 0;
}

void writeToFile(DATEIKARTE *anfang) {
    DATEIKARTE *current = anfang;
    FILE *fptr = fopen("Karteikarten.csv", "w"); // öffnet Datei zum Schreiben ("w" = write), erstellt sie neu falls sie existiert
    if (fptr == NULL) {
        printf("Fehler beim Öffnen der Datei!\n");
        return;
    }

    while (current != NULL) {
        fprintf(fptr, "%s;", current->frage);   // schreibt "frage;" in die Datei
        fprintf(fptr, "%s\n", current->antwort); // schreibt "antwort\n" in die Datei
        current = current->next; // current-> greift auf das Feld des Structs zu, auf den der Pointer zeigt
    }

    fclose(fptr); // Datei schließen
}

void deleteFile()
{
    remove("Karteikarten.csv"); // löscht die Datei vom Dateisystem
}