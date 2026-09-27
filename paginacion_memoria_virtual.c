/* ============================================================================
 * SISTEMAS OPERATIVOS
 * Ejercicio 2: Memoria virtual - Paginacion
 * Traduccion de direcciones logicas a fisicas y reemplazo de paginas (FIFO)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define TAM_PAGINA   256   /* bytes por pagina/marco               */
#define NUM_PAGINAS  8     /* paginas logicas del proceso           */
#define NUM_MARCOS   4     /* marcos fisicos disponibles (< NUM_PAGINAS) */

int tabla_paginas[NUM_PAGINAS];
int cola_marcos[NUM_MARCOS];
int marcos_ocupados = 0;
int siguiente_marco_a_reemplazar = 0;

int contador_fallos = 0;
int contador_aciertos = 0;

void inicializar_tabla_paginas(void) {
    int i;
    for (i = 0; i < NUM_PAGINAS; i++) {
        tabla_paginas[i] = -1;
    }
    for (i = 0; i < NUM_MARCOS; i++) {
        cola_marcos[i] = -1;
    }
    marcos_ocupados = 0;
    siguiente_marco_a_reemplazar = 0;
    contador_fallos = 0;
    contador_aciertos = 0;
}

void mostrar_tabla_paginas(void) {
    int i;
    printf("\n--- Tabla de paginas (proceso) ---\n");
    for (i = 0; i < NUM_PAGINAS; i++) {
        if (tabla_paginas[i] == -1) {
            printf(" Pagina %d -> no cargada\n", i);
        } else {
            printf(" Pagina %d -> Marco %d\n", i, tabla_paginas[i]);
        }
    }
    printf("--- Marcos fisicos ---\n");
    for (i = 0; i < NUM_MARCOS; i++) {
        if (cola_marcos[i] == -1) {
            printf(" Marco %d -> libre\n", i);
        } else {
            printf(" Marco %d -> Pagina %d\n", i, cola_marcos[i]);
        }
    }
    printf("Fallos de pagina: %d | Aciertos: %d\n", contador_fallos, contador_aciertos);
}

/* Carga una pagina en un marco fisico (libre o reemplazando por FIFO). */
int cargar_pagina_en_memoria(int pagina) {
    int marco_elegido;
    int pagina_reemplazada;

    if (marcos_ocupados < NUM_MARCOS) {
        /* Todavia hay marcos libres: se usa el siguiente disponible */
        marco_elegido = marcos_ocupados;
        marcos_ocupados++;
    } else {
        /* Memoria llena: reemplazo FIFO */
        marco_elegido = siguiente_marco_a_reemplazar;
        pagina_reemplazada = cola_marcos[marco_elegido];
        tabla_paginas[pagina_reemplazada] = -1;
        printf("  >> Memoria llena: FIFO expulsa la pagina %d del marco %d\n",
               pagina_reemplazada, marco_elegido);
        siguiente_marco_a_reemplazar = (siguiente_marco_a_reemplazar + 1) % NUM_MARCOS;
    }

    cola_marcos[marco_elegido] = pagina;
    tabla_paginas[pagina] = marco_elegido;

    return marco_elegido;
}

/* Traduce una direccion logica a fisica, contando aciertos y fallos. */
int traducir_direccion(int direccion_logica) {
    int numero_pagina;
    int desplazamiento;
    int marco;
    int direccion_fisica;

    /* Se valida ANTES de dividir: en C, -1 / 256 daria 0 (valido por error) */
    if (direccion_logica < 0 || direccion_logica >= TAM_PAGINA * NUM_PAGINAS) {
        printf("Direccion logica %d invalida (rango permitido: 0 - %d)\n",
               direccion_logica, TAM_PAGINA * NUM_PAGINAS - 1);
        return -1;
    }

    numero_pagina  = direccion_logica / TAM_PAGINA;
    desplazamiento = direccion_logica % TAM_PAGINA;

    if (tabla_paginas[numero_pagina] != -1) {
        marco = tabla_paginas[numero_pagina];
        contador_aciertos++;
        printf("[ACIERTO] La pagina %d ya estaba en el marco %d\n", numero_pagina, marco);
    } else {
        contador_fallos++;
        printf("[FALLO DE PAGINA] La pagina %d no esta en memoria\n", numero_pagina);
        marco = cargar_pagina_en_memoria(numero_pagina);
    }

    direccion_fisica = marco * TAM_PAGINA + desplazamiento;

    printf("Direccion logica %d -> pagina %d, desplazamiento %d -> "
           "marco %d -> direccion fisica %d\n\n",
           direccion_logica, numero_pagina, desplazamiento, marco, direccion_fisica);

    return direccion_fisica;
}

int main() {
    int secuencia[] = {0, 300, 600, 900, 1200, 1500, 50, 1800, 610, 2000};
    int total = sizeof(secuencia) / sizeof(secuencia[0]);
    int i, opcion, direccion;

    inicializar_tabla_paginas();

    printf("=============================================================\n");
    printf(" SIMULADOR DE MEMORIA VIRTUAL - PAGINACION CON REEMPLAZO FIFO\n");
    printf(" Tamano de pagina: %d bytes | Paginas logicas: %d | Marcos: %d\n",
           TAM_PAGINA, NUM_PAGINAS, NUM_MARCOS);
    printf("=============================================================\n");

    do {
        printf("\n1. Ejecutar secuencia de accesos de ejemplo\n");
        printf("2. Traducir una direccion logica manualmente\n");
        printf("3. Ver tabla de paginas y marcos\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                for (i = 0; i < total; i++) {
                    traducir_direccion(secuencia[i]);
                }
                mostrar_tabla_paginas();
                break;
            case 2:
                printf("Ingrese direccion logica (0 - %d): ", TAM_PAGINA * NUM_PAGINAS - 1);
                scanf("%d", &direccion);
                traducir_direccion(direccion);
                break;
            case 3:
                mostrar_tabla_paginas();
                break;
            case 0:
                printf("Saliendo del simulador...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 0);

    return 0;
}
