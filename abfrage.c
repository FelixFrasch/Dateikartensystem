#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "abfrage.h"

// --- Hilfsfunktionen ---

// Groß-/Kleinschreibung ignorierender Stringvergleich
static int vergleicheIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return tolower((unsigned char)*a) - tolower((unsigned char)*b);
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

// Prüft ob die erwartete Antwort eine Ja/Nein-Antwort ist
static int istJaNeinAntwort(const char *antwort) {
    char lower[ALENGTH];
    int i;
    for (i = 0; antwort[i] && i < ALENGTH - 1; i++)
        lower[i] = (char)tolower((unsigned char)antwort[i]);
    lower[i] = '\0';
    return (strcmp(lower, "ja") == 0 || strcmp(lower, "nein") == 0 ||
            strcmp(lower, "yes") == 0 || strcmp(lower, "no") == 0 ||
            strcmp(lower, "richtig") == 0 || strcmp(lower, "falsch") == 0);
}

// Normalisiert Ja/Nein-Varianten auf "ja" oder "nein"
// Akzeptiert: j/ja/y/yes/richtig/wahr -> "ja"; n/nein/no/falsch -> "nein"
static void normalisiereJaNein(char *s) {
    char lower[ALENGTH];
    int i;
    for (i = 0; s[i] && i < ALENGTH - 1; i++)
        lower[i] = (char)tolower((unsigned char)s[i]);
    lower[i] = '\0';

    if (strcmp(lower, "j") == 0 || strcmp(lower, "ja") == 0 ||
        strcmp(lower, "y") == 0 || strcmp(lower, "yes") == 0 ||
        strcmp(lower, "richtig") == 0 || strcmp(lower, "wahr") == 0) {
        strcpy(s, "ja");
    } else if (strcmp(lower, "n") == 0 || strcmp(lower, "nein") == 0 ||
               strcmp(lower, "no") == 0 || strcmp(lower, "falsch") == 0) {
        strcpy(s, "nein");
    }
    // Unbekannte Eingaben bleiben unverändert und schlagen beim Vergleich fehl
}

static int zaehleKarten(DATEIKARTE *anfang) {
    int n = 0;
    while (anfang) { n++; anfang = anfang->next; }
    return n;
}

// Führt einen Abfrage-Durchlauf durch.
// pool:         Array von Karten-Pointern
// n:            Anzahl Karten im Pool
// falschPool:   Ausgabe-Array für falsch beantwortete Karten (muss n Plätze haben)
// falschAnzahl: Ausgabe: Anzahl falsch beantworteter Karten
// abgebrochen:  Ausgabe: 1 wenn Nutzer mit 'q' abgebrochen hat
// Rückgabe:     Anzahl richtig beantworteter Karten
static int durchlauf(DATEIKARTE **pool, int n, DATEIKARTE **falschPool,
                     int *falschAnzahl, int *abgebrochen) {
    int richtig = 0;
    *falschAnzahl = 0;
    *abgebrochen = 0;

    for (int i = 0; i < n; i++) {
        DATEIKARTE *karte = pool[i];
        char antwort[ALENGTH];

        printf("\n[%d/%d] Frage: %s\n", i + 1, n, karte->inhalt->frage);

        while (1) {
            printf("Antwort (h=Hinweis, q=Beenden): ");
            if (fgets(antwort, sizeof(antwort), stdin) == NULL) {
                *abgebrochen = 1;
                return richtig;
            }
            antwort[strcspn(antwort, "\n")] = '\0';

            if (strcmp(antwort, "q") == 0) {
                *abgebrochen = 1;
                return richtig;
            }

            // Hinweis: ersten Buchstaben der Antwort zeigen
            if (strcmp(antwort, "h") == 0) {
                printf("Hinweis: Die Antwort beginnt mit '%c'\n", karte->inhalt->antwort[0]);
                continue;
            }

            // Kopien für die Normalisierung anlegen (Original bleibt unberührt)
            char antwortNorm[ALENGTH];
            strncpy(antwortNorm, antwort, ALENGTH - 1);
            antwortNorm[ALENGTH - 1] = '\0';

            char erwartetNorm[ALENGTH];
            strncpy(erwartetNorm, karte->inhalt->antwort, ALENGTH - 1);
            erwartetNorm[ALENGTH - 1] = '\0';

            // Ja/Nein-Normalisierung, falls die erwartete Antwort eine Ja/Nein-Antwort ist
            if (istJaNeinAntwort(karte->inhalt->antwort)) {
                normalisiereJaNein(antwortNorm);
                normalisiereJaNein(erwartetNorm);
            }

            if (vergleicheIgnoreCase(antwortNorm, erwartetNorm) == 0) {
                printf("Richtig!\n");
                richtig++;
            } else {
                printf("Falsch! Erwartet: %s\n", karte->inhalt->antwort);
                falschPool[*falschAnzahl] = karte;
                (*falschAnzahl)++;
            }
            break; // weiter zur nächsten Karte
        }
    }
    return richtig;
}

// --- Hauptfunktion ---

void abfrageStarten(DATEIKARTE *anfang) {
    if (anfang == NULL) {
        printf("Liste ist leer.\n");
        return;
    }

    int gesamt = zaehleKarten(anfang);

    // Abfragebereich wählen (alle oder ID-Bereich)
    printf("Alle %d Karten abfragen? (j/n): ", gesamt);
    char eingabe[20];
    if (fgets(eingabe, sizeof(eingabe), stdin) == NULL) return;
    eingabe[strcspn(eingabe, "\n")] = '\0';

    int alleKarten;
    {
        char tmp[20];
        strncpy(tmp, eingabe, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';
        for (int i = 0; tmp[i]; i++) tmp[i] = (char)tolower((unsigned char)tmp[i]);
        alleKarten = !(strcmp(tmp, "n") == 0 || strcmp(tmp, "nein") == 0 || strcmp(tmp, "no") == 0);
    }

    int vonId = 0, bisId = 0;
    if (!alleKarten) {
        char buf[30];
        printf("Von ID: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) return;
        if (sscanf(buf, "%d", &vonId) != 1) { printf("Ungültige Eingabe.\n"); return; }
        printf("Bis ID: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) return;
        if (sscanf(buf, "%d", &bisId) != 1) { printf("Ungültige Eingabe.\n"); return; }
    }

    // Pool aufbauen
    DATEIKARTE **pool = malloc(gesamt * sizeof(DATEIKARTE *));
    if (pool == NULL) { printf("Speicherfehler.\n"); return; }

    int n = 0;
    DATEIKARTE *tmp = anfang;
    while (tmp) {
        if (alleKarten || (tmp->inhalt->id >= vonId && tmp->inhalt->id <= bisId))
            pool[n++] = tmp;
        tmp = tmp->next;
    }

    if (n == 0) {
        printf("Keine Karten in diesem Bereich.\n");
        free(pool);
        return;
    }

    // Zufällige Reihenfolge (Fisher-Yates)
    srand((unsigned int)time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        DATEIKARTE *t = pool[i];
        pool[i] = pool[j];
        pool[j] = t;
    }

    DATEIKARTE **falschPool = malloc(n * sizeof(DATEIKARTE *));
    if (falschPool == NULL) { free(pool); printf("Speicherfehler.\n"); return; }

    printf("\n---------- Abfrage beginnt (%d Karten) ----------\n", n);

    int falschAnzahl = 0;
    int abgebrochen = 0;

    // Erster Durchlauf – dieser zählt für den Score
    int richtig = durchlauf(pool, n, falschPool, &falschAnzahl, &abgebrochen);

    // Falsch beantwortete Karten wiederholen (kein Einfluss auf den Score)
    while (falschAnzahl > 0 && !abgebrochen) {
        printf("\n%d Karte(n) falsch beantwortet – werden wiederholt.\n", falschAnzahl);

        DATEIKARTE **neuPool = malloc(falschAnzahl * sizeof(DATEIKARTE *));
        if (neuPool == NULL) break;
        memcpy(neuPool, falschPool, falschAnzahl * sizeof(DATEIKARTE *));
        int neuN = falschAnzahl;

        // Wiederholung ebenfalls in zufälliger Reihenfolge
        for (int i = neuN - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            DATEIKARTE *t = neuPool[i];
            neuPool[i] = neuPool[j];
            neuPool[j] = t;
        }

        falschAnzahl = 0;
        durchlauf(neuPool, neuN, falschPool, &falschAnzahl, &abgebrochen);
        free(neuPool);
    }

    // Auswertung
    printf("\n---------- Abfrage beendet ----------\n");
    printf("Ergebnis: %d/%d richtig (%.0f%%)\n",
           richtig, n, n > 0 ? (100.0 * richtig / n) : 0.0);

    free(falschPool);
    free(pool);
}
