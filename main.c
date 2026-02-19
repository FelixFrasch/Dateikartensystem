#include <stdio.h>
#include <string.h>

#include "config.h"
#include "liste.h"
#include "fileio.h"
#include "abfrage.h"

void zeigeErgebnis(FEHLERCODE code, const char *aktion);
void frageDateiname(char *out, int maxLen);
void deleteElementFromTerminal(DATEIKARTE **anfang);
void addElementFromTerminal(DATEIKARTE **anfang);
void printInfo();
void clearTerminal(int count);


int main(void) {
    char userInput = 0;
    clearTerminal(40);
    printInfo();
    DATEIKARTE *dateikarte = NULL; // Pointer auf das erste Element der Liste, NULL = leere Liste

    while (1) {

        clearTerminal(40);
        printInfo();
        printf("Auswahl: ");
        if (scanf(" %c", &userInput) != 1) { // Leerzeichen vor %c überspringt Whitespace (z.B. vorheriges \n)
            while (getchar() != '\n') {} // Eingabepuffer leeren
            printf("Ungültige Eingabe. Gebe einen char ein.\n");
            continue;
        }
        while (getchar() != '\n') {} // stellt sicher, dass der Eingabepuffer leer ist

        if (userInput) {
            char dateiname[260]; // Puffer für variablen Dateinamen
            switch (userInput) {
        case 'q': deleteList(&dateikarte); // Programm beenden
                return 0;
        case '1': addElementFromTerminal(&dateikarte); // Element hinzufügen
                break;
        case '2': printList(dateikarte); // gesamte Liste ausgeben
                break;
        case '3': deleteElementFromTerminal(&dateikarte); // beliebiges Element löschen
                break;
        case '4': deleteList(&dateikarte); // Liste löschen
                break;
        case '5': sortList(&dateikarte, 0); // nach Frage sortieren
                break;
        case 's': sortList(&dateikarte, 1); // nach ID sortieren
                break;
        case 'w': // in Datei schreiben
                frageDateiname(dateiname, sizeof(dateiname));
                if (dateiname[0] == '\0') break;
                zeigeErgebnis(writeToFile(dateikarte, dateiname), "Speichern");
                break;
        case 'r': // aus Datei lesen
                frageDateiname(dateiname, sizeof(dateiname));
                if (dateiname[0] == '\0') break;
                zeigeErgebnis(readFromFile(&dateikarte, dateiname), "Laden");
                break;
        case 'd': // Datei löschen
                frageDateiname(dateiname, sizeof(dateiname));
                if (dateiname[0] == '\0') break;
                zeigeErgebnis(deleteFile(dateiname), "Datei löschen");
                break;
        case 'a': abfrageStarten(dateikarte); // Abfrage starten
                break;
        default: printf("Ungültige Eingabe.\n");
                break;
            }
        }
        printf("Drücke Enter, um weiter zu machen...");
        getchar(); // wartet auf Enter

    }
}

// Gibt eine einheitliche Erfolgs- oder Fehlermeldung aus.
// aktion beschreibt den Vorgang (z.B. "Speichern"), der in der Meldung erscheint.
void zeigeErgebnis(FEHLERCODE code, const char *aktion) {
    switch (code) {
    case OK:                    printf("%s erfolgreich.\n", aktion);                        break;
    case FEHLER_SPEICHER:       printf("%s fehlgeschlagen: Kein Speicher verfügbar.\n", aktion); break;
    case FEHLER_LISTE_LEER:     printf("%s fehlgeschlagen: Liste ist leer.\n", aktion);    break;
    case FEHLER_NICHT_GEFUNDEN: printf("%s fehlgeschlagen: Element nicht gefunden.\n", aktion); break;
    case FEHLER_DATEI:          printf("%s fehlgeschlagen: Dateifehler.\n", aktion);       break;
    default:                    printf("%s: Unbekannter Fehler.\n", aktion);               break;
    }
}

// Fragt nach einem Dateinamen und stellt sicher, dass er auf .csv endet.
// Schreibt das Ergebnis in out (maxLen Bytes inkl. '\0').
void frageDateiname(char *out, int maxLen) {
    printf("Dateiname (Enter für \"%s\"): ", DATEINAME);
    if (fgets(out, maxLen, stdin) == NULL) { out[0] = '\0'; return; }
    out[strcspn(out, "\n")] = '\0'; // \n entfernen

    if (out[0] == '\0') { // leere Eingabe -> Standardname
        strncpy(out, DATEINAME, maxLen - 1);
        out[maxLen - 1] = '\0';
        return;
    }

    // .csv anhängen, falls die Erweiterung fehlt
    int len = (int)strlen(out);
    if (len < 4 || strcmp(out + len - 4, ".csv") != 0) {
        if (len + 4 < maxLen)
            strcat(out, ".csv");
    }
}

void deleteElementFromTerminal(DATEIKARTE **anfang) {
    int id;
    char buffer[100];

    printf("\n---------- Karte wird entfernt ----------\n");
    printf("Gebe die Id an: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) { // fgets gibt NULL bei Fehler/EOF zurück
        printf("Fehler beim Lesen.\n");
        return;
    }

    if (sscanf(buffer, "%d", &id) != 1) { // sscanf liest aus String, gibt Anzahl gelesener Felder zurück
        printf("Das war keine gültige Zahl!\n");
        return;
    }

    // Eigene Meldungen, um die ID in der Ausgabe zu zeigen
    switch (deleteElement(anfang, id)) {
    case OK:                    printf("Element mit ID %d gelöscht.\n", id);         break;
    case FEHLER_LISTE_LEER:     printf("Die Liste ist leer!\n");                     break;
    case FEHLER_NICHT_GEFUNDEN: printf("Element mit ID %d nicht gefunden!\n", id);   break;
    default:                    printf("Unbekannter Fehler.\n");                     break;
    }
}

void addElementFromTerminal(DATEIKARTE **anfang) {
    char frage[FLENGTH];
    char antwort[ALENGTH];
    printf("\n");
    printf("---------- Neue Karte wird hinzugefügt ----------\n");

    printf("Gebe die Frage an: ");
    if (fgets(frage, sizeof(frage), stdin) == NULL) return;
    if (strchr(frage, '\n') == NULL) { // kein \n -> Buffer voll -> Eingabe zu lang
        while (getchar() != '\n') {} // Eingabepuffer leeren
        printf("Eingabe zu lang! Maximal %d Zeichen erlaubt.\n", FLENGTH - 1);
        return;
    }
    frage[strcspn(frage, "\n")] = '\0'; // strcspn findet Position von \n, wird mit \0 überschrieben

    printf("Gebe die Antwort an: ");
    if (fgets(antwort, sizeof(antwort), stdin) == NULL) return;
    if (strchr(antwort, '\n') == NULL) {
        while (getchar() != '\n') {}
        printf("Eingabe zu lang! Maximal %d Zeichen erlaubt.\n", ALENGTH - 1);
        return;
    }
    antwort[strcspn(antwort, "\n")] = '\0';

    zeigeErgebnis(addElement(anfang, frage, antwort), "Karte hinzufügen");
}

void printInfo() {
    static char a[] = "---------- Dateikartensystem von Felix Frasch ----------\n"
                      "Schreibe 1, um ein Element zur Liste hinzu zu fügen.\n"
                      "Schreibe 2, um die Liste auszugeben.\n"
                      "Schreibe 3, um ein beliebiges Element aus der Liste zu löschen.\n"
                      "Schreibe 4, um die Liste zu löschen.\n"
                      "Schreibe 5, um nach Frage zu sortieren.\n"
                      "Schreibe s, um nach ID zu sortieren.\n"
                      "Schreibe w, um die Karteikarten zu speichern.\n"
                      "Schreibe r, um Karteikarten aus dem Speicher zu laden.\n"
                      "Schreibe d, um die Datei zu löschen.\n"
                      "Schreibe a, um die Abfrage zu starten.\n"
                      "Schreibe q, um abzubrechen.\n";
    printf("%s", a);
}

void clearTerminal(const int count) {
    for (int i = 0; i < count; ++i)
        printf("\n");
}
