#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "liste.h"
#include "fileio.h"
#include "abfrage.h"

// --- Zustandsvariablen der aktuellen Sitzung ---
static int  dirty = 0;              // 1 = Änderungen seit dem letzten Speichern vorhanden
static char aktDateiname[260] = ""; // zuletzt gespeicherter oder geladener Dateiname

// --- Vorwärtsdeklarationen ---
void zeigeErgebnis(FEHLERCODE code, const char *aktion);
void frageDateiname(char *out, int maxLen);
FEHLERCODE deleteElementFromTerminal(DATEIKARTE **anfang);
FEHLERCODE editElementFromTerminal(DATEIKARTE **anfang);
void addElementFromTerminal(DATEIKARTE **anfang);
void printInfo(void);
void clearTerminal(int count);

// Fragt den Benutzer nach einer Ja/Nein-Bestätigung.
// Gibt 1 zurück bei "j"/"ja", sonst 0.
static int frageBestaetige(const char *meldung) {
    char buf[10];
    printf("%s (j/n): ", meldung);
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    for (int i = 0; buf[i]; i++) buf[i] = (char)tolower((unsigned char)buf[i]);
    return strcmp(buf, "j") == 0 || strcmp(buf, "ja") == 0;
}

// Gibt 1 zurück wenn die angegebene Datei auf dem Dateisystem existiert.
static int dateiExistiert(const char *name) {
    FILE *f = fopen(name, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

// Speichert die Liste automatisch in aktDateiname, falls dieser gesetzt ist.
// Setzt dirty=0 und gibt eine Meldung aus bei Erfolg.
static void autoSpeichern(DATEIKARTE *anfang) {
    if (aktDateiname[0] == '\0') return; // kein Dateiname gesetzt, nichts zu tun
    if (writeToFile(anfang, aktDateiname) == OK) {
        dirty = 0;
        printf("(Automatisch gespeichert in \"%s\")\n", aktDateiname);
    } else {
        printf("(Auto-Speichern in \"%s\" fehlgeschlagen!)\n", aktDateiname);
    }
}


int main(void) {
    char userInput = 0;
    DATEIKARTE *dateikarte = NULL; // Pointer auf das erste Element der Liste, NULL = leere Liste

    while (1) {
        clearTerminal(0);
        printInfo();
        if (dirty)
            printf("[Ungespeicherte Aenderungen vorhanden]\n");
        if (aktDateiname[0] != '\0')
            printf("[Aktuelle Datei: %s]\n", aktDateiname);

        printf("Auswahl: ");
        if (scanf(" %c", &userInput) != 1) { // Leerzeichen vor %c überspringt Whitespace (z.B. vorheriges \n)
            while (getchar() != '\n') {} // Eingabepuffer leeren
            printf("Ungültige Eingabe. Gebe einen char ein.\n");
            continue;
        }
        while (getchar() != '\n') {} // stellt sicher, dass der Eingabepuffer leer ist

        if (userInput) {
            char dateiname[260]; // Puffer für variablen Dateinamen
            FEHLERCODE code;
            switch (userInput) {

        case 'q': // Programm beenden – bei ungespeicherten Änderungen nachfragen
                if (!dirty || frageBestaetige("Ungespeicherte Aenderungen verwerfen und beenden?")) {
                    deleteList(&dateikarte);
                    return 0;
                }
                break;

        case '1': // Element hinzufügen (mit Auto-Save nach jeder neuen Karte)
                addElementFromTerminal(&dateikarte);
                break;

        case '2': // gesamte Liste ausgeben
                printList(dateikarte);
                break;

        case '3': // beliebiges Element löschen
                if (deleteElementFromTerminal(&dateikarte) == OK)
                    dirty = 1;
                break;

        case '4': // Liste löschen
                if (dateikarte == NULL) {
                    printf("Liste ist bereits leer.\n");
                } else if (frageBestaetige("Liste wirklich loeschen?")) {
                    deleteList(&dateikarte);
                    dirty = 0;          // leere Liste hat keine ungespeicherten Daten
                    aktDateiname[0] = '\0';
                    printf("Liste gelöscht.\n");
                }
                break;

        case '5': // nach Frage sortieren
                if (dateikarte != NULL) { sortList(&dateikarte, 0); dirty = 1; }
                break;

        case 's': // nach ID sortieren
                if (dateikarte != NULL) { sortList(&dateikarte, 1); dirty = 1; }
                break;

        case 'e': // Karte bearbeiten
                if (editElementFromTerminal(&dateikarte) == OK) {
                    dirty = 1;
                    autoSpeichern(dateikarte);
                }
                break;

        case 'w': // in Datei schreiben
                frageDateiname(dateiname, sizeof(dateiname));
                if (dateiname[0] == '\0') break;
                // Vor dem Überschreiben einer vorhandenen Datei nachfragen
                if (dateiExistiert(dateiname) &&
                    !frageBestaetige("Datei existiert bereits. Ueberschreiben?"))
                    break;
                code = writeToFile(dateikarte, dateiname);
                zeigeErgebnis(code, "Speichern");
                if (code == OK) {
                    strncpy(aktDateiname, dateiname, sizeof(aktDateiname) - 1);
                    aktDateiname[sizeof(aktDateiname) - 1] = '\0';
                    dirty = 0;
                }
                break;

        case 'r': // aus Datei lesen
                // Ungespeicherte Änderungen würden durch Laden überschrieben – nachfragen
                if (dirty && !frageBestaetige("Ungespeicherte Aenderungen verwerfen und neu laden?"))
                    break;
                frageDateiname(dateiname, sizeof(dateiname));
                if (dateiname[0] == '\0') break;
                code = readFromFile(&dateikarte, dateiname);
                zeigeErgebnis(code, "Laden");
                if (code == OK) {
                    strncpy(aktDateiname, dateiname, sizeof(aktDateiname) - 1);
                    aktDateiname[sizeof(aktDateiname) - 1] = '\0';
                    dirty = 0;
                }
                break;

        case 'd': // Datei löschen
                frageDateiname(dateiname, sizeof(dateiname));
                if (dateiname[0] == '\0') break;
                if (!frageBestaetige("Datei wirklich loeschen?")) break;
                zeigeErgebnis(deleteFile(dateiname), "Datei loeschen");
                // aktDateiname leeren falls die gerade aktive Datei gelöscht wurde
                if (strcmp(dateiname, aktDateiname) == 0)
                    aktDateiname[0] = '\0';
                break;

        case 'a': // Abfrage starten – SM-2 Lernfortschritt wird danach auto-gespeichert
                abfrageStarten(dateikarte);
                if (dateikarte != NULL) {
                    dirty = 1;             // SM-2 Daten haben sich geändert
                    autoSpeichern(dateikarte);
                }
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
    case OK:                    printf("%s erfolgreich.\n", aktion);                            break;
    case FEHLER_SPEICHER:       printf("%s fehlgeschlagen: Kein Speicher verfügbar.\n", aktion); break;
    case FEHLER_LISTE_LEER:     printf("%s fehlgeschlagen: Liste ist leer.\n", aktion);          break;
    case FEHLER_NICHT_GEFUNDEN: printf("%s fehlgeschlagen: Element nicht gefunden.\n", aktion);  break;
    case FEHLER_DATEI:          printf("%s fehlgeschlagen: Dateifehler.\n", aktion);             break;
    default:                    printf("%s: Unbekannter Fehler.\n", aktion);                     break;
    }
}

// Fragt nach einem Dateinamen und stellt sicher, dass er auf .csv endet.
// Leere Eingabe ergibt den Standardnamen aus config.h.
// Falsche Erweiterung (z.B. ".txt") wird durch ".csv" ersetzt.
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

    // Prüfen ob die Datei bereits auf .csv endet
    int len = (int)strlen(out);
    if (len < 4 || strcmp(out + len - 4, ".csv") != 0) {
        char *punkt = strrchr(out, '.'); // letzten Punkt im Dateinamen suchen
        if (punkt != NULL) {
            // Falsche Erweiterung: ab dem Punkt mit .csv überschreiben (z.B. "test.txt" -> "test.csv")
            if ((int)(punkt - out) + 4 < maxLen)
                strcpy(punkt, ".csv");
        } else {
            // Keine Erweiterung: .csv anhängen (z.B. "test" -> "test.csv")
            if (len + 4 < maxLen)
                strcat(out, ".csv");
        }
    }
}

// Löscht ein Element anhand der vom Benutzer eingegebenen ID.
// Gibt OK bei Erfolg zurück, sonst den entsprechenden Fehlercode.
FEHLERCODE deleteElementFromTerminal(DATEIKARTE **anfang) {
    int id;
    char buffer[100];

    printf("\n---------- Karte wird entfernt ----------\n");
    printf("Gebe die Id an: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) { // fgets gibt NULL bei Fehler/EOF zurück
        printf("Fehler beim Lesen.\n");
        return FEHLER_DATEI;
    }

    if (sscanf(buffer, "%d", &id) != 1) { // sscanf liest aus String, gibt Anzahl gelesener Felder zurück
        printf("Das war keine gültige Zahl!\n");
        return FEHLER_NICHT_GEFUNDEN;
    }

    FEHLERCODE code = deleteElement(anfang, id);
    switch (code) {
    case OK:                    printf("Element mit ID %d gelöscht.\n", id);          break;
    case FEHLER_LISTE_LEER:     printf("Die Liste ist leer!\n");                      break;
    case FEHLER_NICHT_GEFUNDEN: printf("Element mit ID %d nicht gefunden!\n", id);    break;
    default:                    printf("Unbekannter Fehler.\n");                      break;
    }
    return code;
}

// Bearbeitet Frage und/oder Antwort einer Karte.
// Leere Eingabe (nur Enter) lässt das jeweilige Feld unverändert.
// Gibt OK bei Erfolg zurück, sonst den entsprechenden Fehlercode.
FEHLERCODE editElementFromTerminal(DATEIKARTE **anfang) {
    int id;
    char buffer[100];

    printf("\n---------- Karte wird bearbeitet ----------\n");
    if (*anfang == NULL) { printf("Liste ist leer.\n"); return FEHLER_LISTE_LEER; }

    printf("Gebe die ID an: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return FEHLER_DATEI;
    if (sscanf(buffer, "%d", &id) != 1) { printf("Keine gültige Zahl.\n"); return FEHLER_NICHT_GEFUNDEN; }

    // Aktuelles Element suchen und Werte anzeigen, bevor neue eingegeben werden
    DATEIKARTE *current = *anfang;
    while (current && current->inhalt->id != id)
        current = current->next;
    if (current == NULL) { printf("ID %d nicht gefunden.\n", id); return FEHLER_NICHT_GEFUNDEN; }

    char neueFrage[FLENGTH];
    printf("Aktuelle Frage:   %s\n", current->inhalt->frage);
    printf("Neue Frage    (Enter = unveraendert): ");
    if (fgets(neueFrage, sizeof(neueFrage), stdin) == NULL) return FEHLER_DATEI;
    if (strchr(neueFrage, '\n') == NULL) { // kein \n -> Buffer voll -> Eingabe zu lang
        while (getchar() != '\n') {}
        printf("Eingabe zu lang! Maximal %d Zeichen.\n", FLENGTH - 1);
        return FEHLER_NICHT_GEFUNDEN;
    }
    neueFrage[strcspn(neueFrage, "\n")] = '\0';

    char neueAntwort[ALENGTH];
    printf("Aktuelle Antwort: %s\n", current->inhalt->antwort);
    printf("Neue Antwort  (Enter = unveraendert): ");
    if (fgets(neueAntwort, sizeof(neueAntwort), stdin) == NULL) return FEHLER_DATEI;
    if (strchr(neueAntwort, '\n') == NULL) {
        while (getchar() != '\n') {}
        printf("Eingabe zu lang! Maximal %d Zeichen.\n", ALENGTH - 1);
        return FEHLER_NICHT_GEFUNDEN;
    }
    neueAntwort[strcspn(neueAntwort, "\n")] = '\0';

    FEHLERCODE code = editElement(anfang, id, neueFrage, neueAntwort);
    zeigeErgebnis(code, "Karte bearbeiten");
    return code;
}

// Liest Frage und Antwort vom Terminal ein und fügt eine neue Karte zur Liste hinzu.
// Speichert nach jedem erfolgreichen Hinzufügen automatisch zwischen.
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

    FEHLERCODE code = addElement(anfang, frage, antwort);
    zeigeErgebnis(code, "Karte hinzufügen");
    if (code == OK) {
        dirty = 1;
        autoSpeichern(*anfang); // nach jeder neuen Karte sofort zwischenspeichern
    }
}

void printInfo(void) {
    static char a[] =
        "---------- Dateikartensystem von Felix Frasch ----------\n"
        "Schreibe 1, um eine Karte hinzuzufuegen.\n"
        "Schreibe 2, um die Liste auszugeben.\n"
        "Schreibe 3, um eine Karte zu loeschen.\n"
        "Schreibe 4, um die Liste zu loeschen.\n"
        "Schreibe 5, um nach Frage zu sortieren.\n"
        "Schreibe s, um nach ID zu sortieren.\n"
        "Schreibe e, um eine Karte zu bearbeiten.\n"
        "Schreibe w, um die Karten zu speichern.\n"
        "Schreibe r, um Karten aus einer Datei zu laden.\n"
        "Schreibe d, um eine Datei zu loeschen.\n"
        "Schreibe a, um die Abfrage zu starten (Anki-Lernlogik).\n"
        "Schreibe q, um das Programm zu beenden.\n";
    printf("%s", a);
}

void clearTerminal(const int count) {
    (void)count;             // Parameter wird für Signatur-Kompatibilität beibehalten
    printf("\033[2J\033[H"); // ANSI: Bildschirminhalt löschen und Cursor auf Position (1,1)
}
