/*
 * Aplicação para Gestão de Tratamentos Aqualin
 * E-fólio B 2525/2526 - Introdução à Programação
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_NOME 100
#define MAX_LINHA 256
#define MAX_CAMARAS 100
#define MAX_AQUALINS 1000
#define MAX_TRATAMENTOS 1000

/* Estrutura para tratamento na câmara */
typedef struct {
    char nome[MAX_NOME];
    int saude_entrada;
    int instante_entrada;
    int instante_saida;
} Tratamento;

/* Estrutura para câmara térmica */
typedef struct {
    char nome[MAX_NOME];
    Tratamento tratamentos[MAX_TRATAMENTOS];
    int num_tratamentos;
} Camara;

/* Estrutura para Aqualin */
typedef struct {
    char nome[MAX_NOME];
    int saude_entrada;
    int saude_atual;
    int instante_entrada;
    int espera;
    int instante_saida;
    char estado[10];  /* "alta" ou "morte" */
    int camara_idx;   /* índice da câmara, -1 se não atribuída */
} Aqualin;

/* Variáveis globais */
Camara camaras[MAX_CAMARAS];
int num_camaras = 0;

Aqualin aqualins[MAX_AQUALINS];
int num_aqualins = 0;

int ultimo_instante = 0;

/* Funções auxiliares */

/* Remove espaços em branco no início e fim da string */
void trim(char *str) {
    int i, j = 0;
    int len = strlen(str);
    
    /* Remove espaços no início */
    for (i = 0; i < len && isspace(str[i]); i++);
    
    /* Copia o resto */
    while (i < len) {
        str[j++] = str[i++];
    }
    str[j] = '\0';
    
    /* Remove espaços no fim */
    j--;
    while (j >= 0 && isspace(str[j])) {
        str[j--] = '\0';
    }
}

/* Valida se o nome tem dois nomes unidos por traço */
bool validar_nome(const char *nome) {
    int traco_pos = -1;
    int len = strlen(nome);
    
    /* Procura o traço */
    for (int i = 0; i < len; i++) {
        if (nome[i] == '-') {
            if (traco_pos != -1) return false;  /* Mais de um traço */
            traco_pos = i;
        }
    }
    
    /* Deve ter exatamente um traço, não no início nem no fim */
    if (traco_pos <= 0 || traco_pos >= len - 1) return false;
    
    /* Verifica que há caracteres antes e depois do traço */
    bool antes_ok = false, depois_ok = false;
    for (int i = 0; i < traco_pos; i++) {
        if (!isspace(nome[i])) {
            antes_ok = true;
            break;
        }
    }
    for (int i = traco_pos + 1; i < len; i++) {
        if (!isspace(nome[i])) {
            depois_ok = true;
            break;
        }
    }
    
    return antes_ok && depois_ok;
}

/* Verifica se uma câmara já existe */
bool camara_existe(const char *nome) {
    for (int i = 0; i < num_camaras; i++) {
        if (strcmp(camaras[i].nome, nome) == 0) return true;
    }
    return false;
}

/* Verifica se um Aqualin já existe */
bool aqualin_existe(const char *nome) {
    for (int i = 0; i < num_aqualins; i++) {
        if (strcmp(aqualins[i].nome, nome) == 0) return true;
    }
    return false;
}

/* Calcula o tempo necessário para levar saúde de 'saude' até 100 */
int calcular_tempo_tratamento(int saude) {
    if (saude >= 100) return 0;
    
    int tics = 0;
    int saude_atual = saude;
    
    while (saude_atual < 100) {
        if (saude_atual > 50) {
            /* +1 ponto por cada tic */
            tics += (100 - saude_atual);
            saude_atual = 100;
        } else if (saude_atual > 20) {
            /* +1 ponto por cada 10 tics - vai até 51 */
            int pontos_ate_51 = 51 - saude_atual;
            tics += pontos_ate_51 * 10;
            saude_atual = 51;
        } else if (saude_atual > 10) {
            /* +1 ponto por cada sol (100 tics) - vai até 21 */
            int pontos_ate_21 = 21 - saude_atual;
            tics += pontos_ate_21 * 100;
            saude_atual = 21;
        } else {
            /* +1 ponto por cada 10 sols (1000 tics) - vai até 11 */
            int pontos_ate_11 = 11 - saude_atual;
            tics += pontos_ate_11 * 1000;
            saude_atual = 11;
        }
    }
    
    return tics;
}

/* Calcula a saúde após 'tempo_espera' tics de espera */
int calcular_degradacao(int saude_inicial, int tempo_espera) {
    int saude = saude_inicial;
    int tics_restantes = tempo_espera;
    
    while (tics_restantes > 0 && saude > 0) {
        if (saude > 50) {
            /* -1 ponto por cada 10 sols (1000 tics) */
            int degradacoes = tics_restantes / 1000;
            if (degradacoes > saude - 50) degradacoes = saude - 50;
            if (degradacoes > 0) {
                saude -= degradacoes;
                tics_restantes -= degradacoes * 1000;
            } else {
                if (tics_restantes >= 1000) {
                    saude--;
                    tics_restantes -= 1000;
                } else {
                    break;
                }
            }
        } else if (saude > 20) {
            /* -1 ponto por cada sol (100 tics) */
            int degradacoes = tics_restantes / 100;
            if (degradacoes > saude - 20) degradacoes = saude - 20;
            if (degradacoes > 0) {
                saude -= degradacoes;
                tics_restantes -= degradacoes * 100;
            } else {
                if (tics_restantes >= 100) {
                    saude--;
                    tics_restantes -= 100;
                } else {
                    break;
                }
            }
        } else if (saude > 10) {
            /* -1 ponto por cada 10 tics */
            int degradacoes = tics_restantes / 10;
            if (degradacoes > saude - 10) degradacoes = saude - 10;
            if (degradacoes > 0) {
                saude -= degradacoes;
                tics_restantes -= degradacoes * 10;
            } else {
                if (tics_restantes >= 10) {
                    saude--;
                    tics_restantes -= 10;
                } else {
                    break;
                }
            }
        } else {
            /* -1 ponto por cada tic */
            int degradacoes = tics_restantes;
            if (degradacoes > saude) degradacoes = saude;
            saude -= degradacoes;
            tics_restantes -= degradacoes;
        }
    }
    
    return (saude > 0) ? saude : 0;
}

/* Adiciona uma câmara */
void adicionar_camara(const char *nome) {
    if (!validar_nome(nome) || camara_existe(nome)) return;
    
    strcpy(camaras[num_camaras].nome, nome);
    camaras[num_camaras].num_tratamentos = 0;
    num_camaras++;
}

/* Adiciona um Aqualin */
void adicionar_aqualin(const char *nome, int saude, int instante) {
    if (!validar_nome(nome)) return;
    if (instante < ultimo_instante) return;
    if (saude <= 0 || saude >= 100) return;
    if (aqualin_existe(nome)) return;
    
    ultimo_instante = instante;
    
    strcpy(aqualins[num_aqualins].nome, nome);
    aqualins[num_aqualins].saude_entrada = saude;
    aqualins[num_aqualins].saude_atual = saude;
    aqualins[num_aqualins].instante_entrada = instante;
    aqualins[num_aqualins].espera = 0;
    aqualins[num_aqualins].instante_saida = -1;
    aqualins[num_aqualins].estado[0] = '\0';
    aqualins[num_aqualins].camara_idx = -1;
    num_aqualins++;
}

/* Estrutura auxiliar para estado das câmaras */
typedef struct {
    int camara_idx;
    int instante_disponivel;
} EstadoCamara;

/* Comparação para ordenação de estados de câmaras */
int comparar_estado_camaras(const void *a, const void *b) {
    EstadoCamara *ea = (EstadoCamara *)a;
    EstadoCamara *eb = (EstadoCamara *)b;
    
    if (ea->instante_disponivel != eb->instante_disponivel)
        return ea->instante_disponivel - eb->instante_disponivel;
    return ea->camara_idx - eb->camara_idx;
}

/* Estrutura auxiliar para altas */
typedef struct {
    int aqualin_idx;
    int instante_saida;
} AltaInfo;

/* Comparação para ordenação de altas */
int comparar_altas(const void *a, const void *b) {
    AltaInfo *ia = (AltaInfo *)a;
    AltaInfo *ib = (AltaInfo *)b;
    
    if (ia->instante_saida != ib->instante_saida)
        return ia->instante_saida - ib->instante_saida;
    return ia->aqualin_idx - ib->aqualin_idx;
}

/* Executa os tratamentos */
void executar_tratamentos() {
    if (num_camaras == 0 || num_aqualins == 0) return;
    
    /* Resetar tratamentos anteriores */
    for (int i = 0; i < num_camaras; i++) {
        camaras[i].num_tratamentos = 0;
    }
    
    /* Estado das câmaras */
    EstadoCamara estado_camaras[MAX_CAMARAS];
    for (int i = 0; i < num_camaras; i++) {
        estado_camaras[i].camara_idx = i;
        estado_camaras[i].instante_disponivel = 0;
    }
    
    /* Listas de altas e mortes */
    AltaInfo altas[MAX_AQUALINS];
    int num_altas = 0;
    int mortes[MAX_AQUALINS];
    int num_mortes = 0;
    int menor_saude_morte = 100, maior_saude_morte = 0;
    
    /* Processar cada Aqualin em ordem FIFO */
    for (int i = 0; i < num_aqualins; i++) {
        /* Ordenar câmaras por disponibilidade */
        qsort(estado_camaras, num_camaras, sizeof(EstadoCamara), comparar_estado_camaras);
        
        int camara_idx = estado_camaras[0].camara_idx;
        int instante_disponivel = estado_camaras[0].instante_disponivel;
        
        /* Calcular tempo de espera */
        int tempo_espera = (instante_disponivel > aqualins[i].instante_entrada) ? 
                          (instante_disponivel - aqualins[i].instante_entrada) : 0;
        aqualins[i].espera = tempo_espera;
        
        /* Calcular saúde após espera */
        int saude_apos_espera = calcular_degradacao(aqualins[i].saude_entrada, tempo_espera);
        aqualins[i].saude_atual = saude_apos_espera;
        
        int instante_inicio = (aqualins[i].instante_entrada > instante_disponivel) ?
                             aqualins[i].instante_entrada : instante_disponivel;
        
        if (saude_apos_espera <= 0) {
            /* Morreu */
            strcpy(aqualins[i].estado, "morte");
            aqualins[i].instante_saida = instante_inicio;
            mortes[num_mortes++] = i;
            
            if (aqualins[i].saude_entrada < menor_saude_morte)
                menor_saude_morte = aqualins[i].saude_entrada;
            if (aqualins[i].saude_entrada > maior_saude_morte)
                maior_saude_morte = aqualins[i].saude_entrada;
        } else {
            /* Tratamento */
            int tempo_tratamento = calcular_tempo_tratamento(saude_apos_espera);
            int instante_alta = instante_inicio + tempo_tratamento;
            
            strcpy(aqualins[i].estado, "alta");
            aqualins[i].instante_saida = instante_alta;
            aqualins[i].camara_idx = camara_idx;
            
            altas[num_altas].aqualin_idx = i;
            altas[num_altas].instante_saida = instante_alta;
            num_altas++;
            
            /* Registrar na câmara */
            int t_idx = camaras[camara_idx].num_tratamentos;
            strcpy(camaras[camara_idx].tratamentos[t_idx].nome, aqualins[i].nome);
            camaras[camara_idx].tratamentos[t_idx].saude_entrada = aqualins[i].saude_entrada;
            camaras[camara_idx].tratamentos[t_idx].instante_entrada = instante_inicio;
            camaras[camara_idx].tratamentos[t_idx].instante_saida = instante_alta;
            camaras[camara_idx].num_tratamentos++;
            
            /* Atualizar disponibilidade da câmara */
            estado_camaras[0].instante_disponivel = instante_alta;
        }
    }
    
    /* Imprimir relatório de altas */
    printf("Altas:\n");
    if (num_altas > 0) {
        /* Ordenar altas */
        qsort(altas, num_altas, sizeof(AltaInfo), comparar_altas);
        
        int primeira_idx = altas[0].aqualin_idx;
        int ultima_idx = altas[num_altas - 1].aqualin_idx;
        
        printf("- primeira: %s %d %d %d %d\n",
               aqualins[primeira_idx].nome,
               aqualins[primeira_idx].saude_entrada,
               aqualins[primeira_idx].instante_entrada,
               aqualins[primeira_idx].espera,
               aqualins[primeira_idx].instante_saida);
        
        printf("- ultima: %s %d %d %d %d\n",
               aqualins[ultima_idx].nome,
               aqualins[ultima_idx].saude_entrada,
               aqualins[ultima_idx].instante_entrada,
               aqualins[ultima_idx].espera,
               aqualins[ultima_idx].instante_saida);
    }
    
    if (num_mortes > 0) {
        printf("Mortes: %d %d %d\n", num_mortes, menor_saude_morte, maior_saude_morte);
    }
}

/* Relatório global */
void relatorio_global() {
    printf("Aqualins: %d\n", num_aqualins);
    printf("Camaras: %d\n", num_camaras);
}

/* Relatório de Aqualins */
void relatorio_aqualins() {
    for (int i = 0; i < num_aqualins; i++) {
        printf("%s %d %d %d",
               aqualins[i].nome,
               aqualins[i].saude_entrada,
               aqualins[i].instante_entrada,
               aqualins[i].espera);
        
        if (strlen(aqualins[i].estado) > 0) {
            printf(" %s:%d", aqualins[i].estado, aqualins[i].instante_saida);
        }
        printf("\n");
    }
}

/* Relatório de câmaras */
void relatorio_camaras() {
    for (int i = 0; i < num_camaras; i++) {
        printf("%s:\n", camaras[i].nome);
        for (int j = 0; j < camaras[i].num_tratamentos; j++) {
            printf("- %s %d %d %d\n",
                   camaras[i].tratamentos[j].nome,
                   camaras[i].tratamentos[j].saude_entrada,
                   camaras[i].tratamentos[j].instante_entrada,
                   camaras[i].tratamentos[j].instante_saida);
        }
    }
}

/* Indicador de espera */
void indicador_espera() {
    int total_espera = 0;
    int count = 0;
    
    for (int i = 0; i < num_aqualins; i++) {
        if (strlen(aqualins[i].estado) > 0) {
            total_espera += aqualins[i].espera;
            count++;
        }
    }
    
    if (count > 0) {
        printf("%d\n", total_espera / count);
    } else {
        printf("0\n");
    }
}

/* Indicador de trabalho */
void indicador_trabalho() {
    int total_trabalho = 0;
    
    for (int i = 0; i < num_aqualins; i++) {
        if (strlen(aqualins[i].estado) > 0) {
            total_trabalho += (aqualins[i].instante_saida - aqualins[i].instante_entrada);
        }
    }
    
    printf("%d\n", total_trabalho);
}

/* Indicador de salvos */
void indicador_salvos() {
    int salvos = 0;
    
    for (int i = 0; i < num_aqualins; i++) {
        if (strcmp(aqualins[i].estado, "alta") == 0) {
            salvos++;
        }
    }
    
    printf("%d\n", salvos);
}

/* Processa uma linha de comando */
void processar_comando(char *linha) {
    trim(linha);
    
    /* Verificar se começa com # */
    if (linha[0] != '#') return;
    
    /* Remover o # */
    char *resto = linha + 1;
    trim(resto);
    
    /* Dividir em palavras */
    char palavras[4][MAX_NOME];
    int num_palavras = 0;
    
    char *token = strtok(resto, " ");
    while (token != NULL && num_palavras < 4) {
        strcpy(palavras[num_palavras], token);
        num_palavras++;
        token = strtok(NULL, " ");
    }
    
    if (num_palavras == 0 || num_palavras > 3) return;
    
    /* Processar comando */
    if (num_palavras == 1) {
        /* Pode ser câmara ou tratamentos */
        if (strcmp(palavras[0], "tratamentos") == 0) {
            executar_tratamentos();
        } else {
            /* Adicionar câmara */
            adicionar_camara(palavras[0]);
        }
    } else if (num_palavras == 2) {
        /* Relatório ou indicador */
        if (strcmp(palavras[0], "relatorio") == 0) {
            if (strcmp(palavras[1], "global") == 0) {
                relatorio_global();
            } else if (strcmp(palavras[1], "aqualins") == 0) {
                relatorio_aqualins();
            } else if (strcmp(palavras[1], "camaras") == 0) {
                relatorio_camaras();
            }
        } else if (strcmp(palavras[0], "indicador") == 0) {
            if (strcmp(palavras[1], "espera") == 0) {
                indicador_espera();
            } else if (strcmp(palavras[1], "trabalho") == 0) {
                indicador_trabalho();
            } else if (strcmp(palavras[1], "salvos") == 0) {
                indicador_salvos();
            }
        }
    } else if (num_palavras == 3) {
        /* Adicionar Aqualin */
        int saude = atoi(palavras[1]);
        int instante = atoi(palavras[2]);
        adicionar_aqualin(palavras[0], saude, instante);
    }
}

int main() {
    char linha[MAX_LINHA];
    
    while (fgets(linha, sizeof(linha), stdin)) {
        /* Remove newline */
        linha[strcspn(linha, "\n")] = '\0';
        
        /* Verifica se começa com # */
        trim(linha);
        if (linha[0] != '#') break;
        
        /* Verifica se tem pelo menos uma palavra após o # */
        char *resto = linha + 1;
        trim(resto);
        if (strlen(resto) == 0) break;
        
        processar_comando(linha);
    }
    
    return 0;
}
