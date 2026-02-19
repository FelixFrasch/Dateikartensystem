#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_CONFIG_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_CONFIG_H

#define FLENGTH   100
#define ALENGTH   100
#define DATEINAME "Karteikarten.csv"

// Einheitliche Fehlercodes für alle Funktionen im Projekt.
typedef enum {
    OK = 0,
    FEHLER_SPEICHER,        // malloc fehlgeschlagen
    FEHLER_LISTE_LEER,      // Liste ist leer
    FEHLER_NICHT_GEFUNDEN,  // Element mit gesuchter ID nicht vorhanden
    FEHLER_DATEI,           // Datei konnte nicht geöffnet/gelöscht werden
} FEHLERCODE;

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_CONFIG_H