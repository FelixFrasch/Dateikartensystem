#include <stdio.h>
#include <string.h>
#include "fileio.h"

FEHLERCODE readFromFile(DATEIKARTE **anfang, const char *dateiname) {
    FILE *fptr = fopen(dateiname, "r"); // öffnet Datei zum Lesen, gibt NULL zurück falls nicht vorhanden
    if (fptr == NULL) return FEHLER_DATEI;
    deleteList(anfang); // alte Liste löschen bevor neue Daten geladen werden

    char line[FLENGTH + ALENGTH + 2];
    char frage[FLENGTH];
    char antwort[ALENGTH];

    char format[50];
    sprintf(format, "%%%d[^;];%%%d[^\n]", FLENGTH - 1, ALENGTH - 1); // z.B. "%99[^;];%99[^\n]"

    while (fgets(line, sizeof(line), fptr)) { // liest eine Zeile (leere Datei: Schleife wird nie betreten)
        if (sscanf(line, format, frage, antwort) == 2) { // == 2: beide Felder erfolgreich gelesen
            if (addElement(anfang, frage, antwort) != OK) { // Speicher prüfen
                fclose(fptr);
                return FEHLER_SPEICHER;
            }
        }
    }

    fclose(fptr);
    return OK;
}

FEHLERCODE writeToFile(DATEIKARTE *anfang, const char *dateiname) {
    FILE *fptr = fopen(dateiname, "w"); // "w" erstellt die Datei neu oder überschreibt sie
    if (fptr == NULL) return FEHLER_DATEI;

    DATEIKARTE *current = anfang;
    while (current != NULL) {
        fprintf(fptr, "%s;%s\n", current->inhalt->frage, current->inhalt->antwort);
        current = current->next;
    }

    fclose(fptr);
    return OK;
}

FEHLERCODE deleteFile(const char *dateiname) {
    return remove(dateiname) == 0 ? OK : FEHLER_DATEI; // remove gibt 0 bei Erfolg zurück
}
