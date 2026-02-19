#include <stdio.h>
#include <string.h>

#include "config.h"
#include "liste.h"
#include "fileio.h"
#include "abfrage.h"

void readFromFileTerminal(DATEIKARTE **anfang);
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

        if (userInput)
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
                if (writeToFile(dateikarte, DATEINAME) == 0)
                    printf("Erfolgreich gespeichert.\n");
                else
                    printf("Fehler beim Schreiben der Datei!\n");
                break;
        case 'r': readFromFileTerminal(&dateikarte); // aus Datei lesen
                break;
        case 'd': // Datei löschen
                if (deleteFile(DATEINAME) == 0)
                    printf("Datei gelöscht.\n");
                else
                    printf("Fehler beim Löschen der Datei!\n");
                break;
        case 'a': abfrageStarten(dateikarte); // Abfrage starten
                break;
        default: printf("Ungültige Eingabe.\n");
                break;
            }
        printf("Drücke Enter, um weiter zu machen...");
        getchar(); // wartet auf Enter

    }
}

void readFromFileTerminal(DATEIKARTE **anfang) {
    // Hilfsfunktion, die Rückgabecodes von readFromFile in Texte übersetzt
    int code = readFromFile(anfang, DATEINAME);
    switch (code) {
    case 0: printf("Karteikarten erfolgreich geladen.\n");
            break;
    case 1: printf("Datei nicht gefunden!\n");
            break;
    default: printf("Unbekannter Rückgabewert!\n");
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

    int code = deleteElement(anfang, id);
    switch (code) {
    case 0: printf("Element mit ID %d gelöscht.\n", id); break;
    case 1: printf("Die Liste ist leer!\n"); break;
    case 2: printf("Element mit ID %d nicht gefunden!\n", id); break;
    default: printf("Unbekannter Fehler.\n"); break;
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

    addElement(anfang, frage, antwort);
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
