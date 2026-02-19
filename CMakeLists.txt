cmake_minimum_required(VERSION 3.10)
project(Testataufgabe_FelixFrasch_1_0 C)

set(CMAKE_C_STANDARD 11)

add_executable(Testataufgabe_FelixFrasch_1_0
        main.c
        liste.c
        fileio.c
        abfrage.c
        liste.h
        fileio.h
        abfrage.h
        config.h)
