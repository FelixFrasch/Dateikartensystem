#ifndef TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H
#define TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H

#include "liste.h"

// Liest Karteikarten aus "Karteikarten.csv" und ersetzt die aktuelle Liste. Gibt 0 bei Erfolg, 1 bei Fehler zurück.
int readFromFile(DATEIKARTE **anfang);

// Schreibt alle Karteikarten der Liste im Format "frage;antwort" in "Karteikarten.csv".
void writeToFile(DATEIKARTE *anfang);

// Löscht die Datei "Karteikarten.csv" vom Dateisystem.
void deleteFile();

#endif //TESTATAUFGABE_FELIXFRASCH_1_0_FILEIO_H