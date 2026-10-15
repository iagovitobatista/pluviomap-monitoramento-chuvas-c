/*
 * PLUVIOMAP - Monitoramento de chuvas por bairro
 * Experiencia Pratica III - Algoritmos e Pensamento Computacional
 * Simulacao educativa: 7 dias, 3 bairros, dados em milimetros (mm).
 * Compile: gcc -std=c11 -Wall -Wextra -pedantic pluviomap.c -o pluviomap -lm
 */
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define DIAS 7
#define BAIRROS 3
#define TAM_ENTRADA 128

/* Lê uma linha e descarta caracteres excedentes para nao afetar a proxima leitura. */
int lerLinha(char texto[], size_t capacidade) {
    if (fgets(texto, (int)capacidade, stdin) == NULL) return 0;
    if (strchr(texto, '\n') == NULL) {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) { }
    }
    return 1;
}

/* Recebe um valor finito e nao negativo. O ponto e o separador decimal. */
int lerChuva(float *valor) {
    char texto[TAM_ENTRADA], extra;
    while (1) {
        if (!lerLinha(texto, sizeof texto)) return 0;
        if (sscanf(texto, " %f %c", valor, &extra) == 1 &&
            isfinite(*valor) && *valor >= 0.0f) {
            return 1;
        }
        printf("Entrada invalida. Informe um numero >= 0 (ex.: 12.5): ");
        fflush(stdout);
    }
}

/* Classificacao ilustrativa usada apenas no heatmap textual. */
char faixaChuva(float mm) {
    if (mm == 0.0f) return '.';   /* Sem chuva */
    if (mm <= 10.0f) return 'F';   /* Fraca */
    if (mm <= 30.0f) return 'M';   /* Moderada */
    return 'I';                   /* Intensa */
}

int main(void) {
    float chuvas[DIAS][BAIRROS] = {{0}};
    float totalBairro[BAIRROS] = {0};
    float totalDia[DIAS] = {0};
    float mediaBairro[BAIRROS] = {0};
    int diaMaisChuvoso = 0, bairroMaisChuvoso = 0;
    float volumeGeral = 0.0f;

    printf("============================================\n");
    printf("        PLUVIOMAP - CHUVAS POR BAIRRO\n");
    printf("============================================\n");
    printf("Periodo: %d dias | Bairros: %d\n", DIAS, BAIRROS);
    printf("Informe os volumes em mm (use ponto decimal).\n");

    /* LEITURA: cada linha corresponde a um dia e cada coluna a um bairro. */
    for (int dia = 0; dia < DIAS; dia++) {
        printf("\n--- DIA %d ---\n", dia + 1);
        for (int bairro = 0; bairro < BAIRROS; bairro++) {
            float valor;
            while (1) {
                printf("Chuva no bairro %d (mm): ", bairro + 1);
                fflush(stdout);
                if (!lerChuva(&valor)) {
                    printf("\nEntrada encerrada. Programa finalizado sem relatorio.\n");
                    return 1;
                }
                /* Evita que os acumuladores float ultrapassem seu limite. */
                if (valor <= FLT_MAX - totalDia[dia] &&
                    valor <= FLT_MAX - totalBairro[bairro]) break;
                printf("Valor muito alto para acumular. Tente novamente.\n");
            }
            chuvas[dia][bairro] = valor;
            totalDia[dia] += valor;
            totalBairro[bairro] += valor;
        }
    }

    /* PROCESSAMENTO: calcula medias e encontra o bairro mais chuvoso. */
    for (int bairro = 0; bairro < BAIRROS; bairro++) {
        mediaBairro[bairro] = totalBairro[bairro] / DIAS;
        if (totalBairro[bairro] > totalBairro[bairroMaisChuvoso]) {
            bairroMaisChuvoso = bairro;
        }
    }

    /* Identifica o dia de maior volume somado entre os tres bairros. */
    for (int dia = 0; dia < DIAS; dia++) {
        if (totalDia[dia] > totalDia[diaMaisChuvoso]) {
            diaMaisChuvoso = dia;
        }
    }

    /* Total geral: somatorio dos registros de todos os bairros e dias. */
    double somaGeral = 0.0;
    for (int bairro = 0; bairro < BAIRROS; bairro++) {
        somaGeral += totalBairro[bairro];
    }
    /* Variavel float para manter os resultados do projeto no tipo planejado. */
    if (somaGeral <= FLT_MAX) volumeGeral = (float)somaGeral;

    /* RELATORIO: os indices da matriz sao exibidos com nomes legiveis. */
    printf("\n============= RELATORIO SEMANAL =============\n");
    printf("%-9s %11s %11s %11s %11s\n", "Dia", "Bairro 1", "Bairro 2", "Bairro 3", "Total");
    for (int dia = 0; dia < DIAS; dia++) {
        printf("Dia %-5d", dia + 1);
        for (int bairro = 0; bairro < BAIRROS; bairro++) {
            printf(" %11.2f", chuvas[dia][bairro]);
        }
        printf(" %11.2f\n", totalDia[dia]);
    }
    printf("Unidade: mm (milimetros).\n");

    printf("\n---------- RESUMO POR BAIRRO ----------\n");
    for (int bairro = 0; bairro < BAIRROS; bairro++) {
        printf("Bairro %d: total = %.2f mm | media diaria = %.2f mm\n",
               bairro + 1, totalBairro[bairro], mediaBairro[bairro]);
    }
    printf("Maior acumulado: bairro %d (%.2f mm).\n",
           bairroMaisChuvoso + 1, totalBairro[bairroMaisChuvoso]);
    printf("Dia de maior volume somado: dia %d (%.2f mm).\n",
           diaMaisChuvoso + 1, totalDia[diaMaisChuvoso]);
    if (somaGeral <= FLT_MAX) {
        printf("Soma de todos os registros: %.2f mm.\n", volumeGeral);
    } else {
        printf("Soma de todos os registros: %.2f mm.\n", somaGeral);
    }
    printf("Observacao: a soma entre bairros e um indicador agregado,\n");
    printf("nao a chuva medida em um unico local.\n");

    /* BONUS: o heatmap em texto mostra a intensidade em cada celula da matriz. */
    printf("\n======= HEATMAP TEXTUAL DE CHUVAS =======\n");
    printf("%-9s %9s %9s %9s\n", "Dia", "Bairro 1", "Bairro 2", "Bairro 3");
    for (int dia = 0; dia < DIAS; dia++) {
        printf("Dia %-5d", dia + 1);
        for (int bairro = 0; bairro < BAIRROS; bairro++) {
            printf(" %9c", faixaChuva(chuvas[dia][bairro]));
        }
        printf("\n");
    }
    printf("Legenda: . = 0 mm | F = ate 10 mm | M = acima de 10 ate 30 mm\n");
    printf("         I = acima de 30 mm. Faixas ilustrativas do projeto.\n");
    printf("\nFim do relatorio PluvioMap.\n");
    return 0;
}