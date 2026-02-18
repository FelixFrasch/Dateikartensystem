#include <stdio.h>
#include <string.h>

#include "liste.h"
#include "fileio.h"

void readFromFileTerminal(DATEIKARTE **anfang);
void abfrageStarten(DATEIKARTE *anfang);
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
            // wenn scanf fehlschlägt
            while (getchar() != '\n'); // liest alle Zeichen bis zum Zeilenende, um den Eingabepuffer zu leeren
            printf("Ungültige Eingabe. Gebe einen char ein.\n");
            continue;
        }
        // stellt sicher, dass der input buffer leer ist
        while (getchar() != '\n');


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
        case 'w': writeToFile(dateikarte); // in Datei schreiben
                break;
        case 'r': readFromFileTerminal(&dateikarte); // aus Datei lesen
                break;
        case 'd': deleteFile(); // Datei löschen
                break;
        case 'a': abfrageStarten(dateikarte); // abfrage Starten
                break;
        default: printf("Ungültige Eingabe.\n");
                break;
            }
        printf("Drücke Enter, um weiter zu machen...");
        getchar(); // wartet, bis ein char erkannt wird (Enter)

    }
}

void readFromFileTerminal(DATEIKARTE **anfang) {
        // Hilfsfunktion, die zwischen Terminal und readFromFile steht
        // Zuständig, für korrekte interpretation von Rückgabewerten
        int code = readFromFile(anfang);
        switch (code)
        {
        case 0: printf("Karteikarten erfolgreich aus Datei überschrieben.\n");
                break;
        case 1: printf("Datei nicht gefunden!\n");
                break;
        default: printf("Unbekannter Rückgabewert!\n");
        }
}

void abfrageStarten(DATEIKARTE *anfang) {
        DATEIKARTE *karte = anfang;
        if (anfang == NULL) {
                printf("Liste ist leer.\n");
                return;
        }

        while (1)
        {
                char antwort[ALENGTH];
                printf("\n");
                printf("---------- Abfrage beginnt ----------\n");
                printf("Frage: %s\n", karte->frage);

                printf("Gebe die Antwort ein: ");
                fgets(antwort, sizeof(antwort), stdin); // liest max. sizeof(antwort)-1 Zeichen von der Tastatur
                antwort[strcspn(antwort, "\n")] = '\0';  // strcspn findet Position von \n, wird mit \0 überschrieben
                if (strcmp(antwort, "q") == 0) return; // strcmp gibt 0 zurück wenn beide Strings identisch sind

                if (strcmp(antwort, karte->antwort) == 0)
                {
                        printf("Richtig!\n");
                } else
                {
                        printf("Falsch!\n");
                        printf("Erwartet: %s\n", karte->antwort);
                }

                if (karte->next == NULL) return;
                karte = karte->next;

        }

}

void deleteElementFromTerminal(DATEIKARTE **anfang) {
        int id;
        char buffer[100];

        printf("\n---------- Karte wird entfernt ----------\n");
        printf("Gebe die Id an: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) { // fgets liest eine Zeile, gibt NULL bei Fehler/EOF zurück
                printf("Fehler beim Lesen.\n");
                return;
        }

        if (sscanf(buffer, "%d", &id) != 1) { // sscanf liest aus einem String statt aus stdin, %d parst eine Ganzzahl, gibt Anzahl gelesener Felder zurück
                printf("Das war keine gültige Zahl!\n");
                return;
        }

        deleteElement(anfang, id);
}

void addElementFromTerminal(DATEIKARTE **anfang) {
        char frage[FLENGTH];
        char antwort[ALENGTH];
        printf("\n");
        printf("---------- Neue Karte wird hinzugefügt ----------\n");

        printf("Gebe die Frage an: ");
        if (fgets(frage, sizeof(frage), stdin) == NULL) return; // liest Eingabe, NULL bei Fehler
        if (strchr(frage, '\n') == NULL) { // kein \n -> Buffer voll -> Eingabe zu lang
            while (getchar() != '\n') {} // Eingabepuffer leeren
            printf("Eingabe zu lang! Maximal %d Zeichen erlaubt.\n", FLENGTH - 1);
            return;
        }
        frage[strcspn(frage, "\n")] = '\0';  // strcspn findet Position von \n, wird mit \0 überschrieben

        printf("Gebe die Antwort an: ");
        if (fgets(antwort, sizeof(antwort), stdin) == NULL) return;
        if (strchr(antwort, '\n') == NULL) { // kein \n -> Buffer voll -> Eingabe zu lang
            while (getchar() != '\n') {} // Eingabepuffer leeren
            printf("Eingabe zu lang! Maximal %d Zeichen erlaubt.\n", ALENGTH - 1);
            return;
        }
        antwort[strcspn(antwort, "\n")] = '\0';

        addElement(anfang, frage, antwort);

}

void printInfo() {
        static char a [] = "---------- Dateikartensystem von Felix Frasch ----------\n"
                           "Schreibe 1, um ein Element zur Liste hinzu zu fügen.\n"
                           "Schreibe 2, um die Liste auszugeben.\n"
                           "Schreibe 3, um ein beliebiges Element aus der Liste zu löschen.\n"
                           "Schreibe 4, um die Liste zu löschen.\n"
                           "Schreibe 5, um nach Frage zu sortieren.\n"
                           "Schreibe s, um nach ID zu sortieren.\n"
                           "Schreibe w, um die Karteikarten zu speichern.\n"
                           "Schreibe r, um eine Datei auszulesen und den Inhalt der List anzuhängen.\n"
                           "Schreibe d, um die Datei zu löchen.\n"
                           "Schreibe a, um die Abfrage zu starten.\n"
                           "Schreibe q, um abzubrechen.\n";
        printf("%s", a);
}

void clearTerminal(const int count) {
        for (int i = 0; i < count; ++i) {
                printf("\n");
        }

}