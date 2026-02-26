#include <stdio.h>
#include <string.h>
#include "fileio.h"

// Datei-Format: frage SEP antwort SEP intervall SEP wiederholungen SEP efFaktor SEP naechsteAbfrage
// SEP ist ASCII 31 (Unit Separator, \x1F) – kann nicht über normale Tastaturen eingegeben werden,
// weshalb Semikolons, Sonderzeichen etc. in Frage und Antwort problemlos gespeichert werden können.

FEHLERCODE readFromFile(DATEIKARTE **anfang, const char *dateiname) {
    FILE *fptr = fopen(dateiname, "r"); // öffnet Datei zum Lesen, gibt NULL zurück falls nicht vorhanden
    if (fptr == NULL) return FEHLER_DATEI;
    deleteList(anfang); // alte Liste löschen bevor neue Daten geladen werden

    char line[FLENGTH + ALENGTH + 64]; // extra Platz für die Lernfelder
    char frage[FLENGTH];
    char antwort[ALENGTH];
    int   intervall, wiederholungen;
    float efFaktor;
    long  naechsteAbfrage; // long für portablen sscanf mit %ld (time_t ist plattformabhängig)

    // Format: frage SEP antwort SEP intervall SEP wiederholungen SEP efFaktor SEP naechsteAbfrage
    // [^\x1F\n] stoppt bei SEP ODER Zeilenumbruch; das erlaubt Semikolons in Frage/Antwort
    char format[128];
    sprintf(format, "%%%d[^\x1F]\x1F%%%d[^\x1F\n]\x1F%%d\x1F%%d\x1F%%f\x1F%%ld",
            FLENGTH - 1, ALENGTH - 1);

    while (fgets(line, sizeof(line), fptr)) { // liest eine Zeile
        int gelesen = sscanf(line, format,
                             frage, antwort,
                             &intervall, &wiederholungen, &efFaktor, &naechsteAbfrage);

        if (gelesen < 2) continue; // unvollständige Zeile überspringen

        if (addElement(anfang, frage, antwort) != OK) { // Speicher prüfen
            fclose(fptr);
            return FEHLER_SPEICHER;
        }

        // Lernfelder nur setzen wenn alle 6 Felder gelesen wurden (neue Dateien)
        // Ansonsten bleiben die Standardwerte aus addElement erhalten (alte Dateien)
        if (gelesen >= 6) {
            DATEIKARTE *last = *anfang;
            while (last->next != NULL) last = last->next; // letztes Element finden
            last->inhalt->intervall       = intervall;
            last->inhalt->wiederholungen  = wiederholungen;
            last->inhalt->efFaktor        = efFaktor;
            last->inhalt->naechsteAbfrage = (time_t)naechsteAbfrage;
        }
    }

    fclose(fptr);
    return OK;
}

FEHLERCODE writeToFile(DATEIKARTE *anfang, const char *dateiname) {
    if (anfang == NULL) return FEHLER_LISTE_LEER; // keine leere Datei anlegen

    FILE *fptr = fopen(dateiname, "w"); // "w" erstellt die Datei neu oder überschreibt sie
    if (fptr == NULL) return FEHLER_DATEI;

    DATEIKARTE *current = anfang;
    while (current != NULL) {
        KARTENINHALT *k = current->inhalt;
        // Alle Felder inkl. Lernfortschritt speichern; SEP (\x1F) als Trenner
        fprintf(fptr, "%s" SEP "%s" SEP "%d" SEP "%d" SEP "%.4f" SEP "%ld\n",
                k->frage, k->antwort,
                k->intervall, k->wiederholungen,
                k->efFaktor, (long)k->naechsteAbfrage);
        current = current->next;
    }

    fclose(fptr);
    return OK;
}

FEHLERCODE deleteFile(const char *dateiname) {
    return remove(dateiname) == 0 ? OK : FEHLER_DATEI; // remove gibt 0 bei Erfolg zurück
}
