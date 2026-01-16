# Solução E-fólio B 2525/2526 - Aplicação para Gestão de Tratamentos Aqualin

## Descrição do Problema

Este exercício pede a implementação de uma aplicação para gerir tratamentos de Aqualins (seres aquáticos fictícios) em câmaras térmicas. A aplicação deve processar comandos que começam com `#` e têm entre 1 a 3 palavras.

## Estrutura da Solução

A solução foi implementada em Python no arquivo `aqualin_treatment.py`.

### Classes Principais

1. **Camara**: Representa uma câmara térmica
   - Atributos: nome, lista de tratamentos realizados

2. **Aqualin**: Representa um paciente Aqualin
   - Atributos: nome, saúde de entrada, saúde atual, instante de entrada, tempo de espera, instante de saída, estado (alta/morte), câmara atribuída

### Funções Principais

1. **validar_nome(nome)**: Valida se o nome tem dois nomes unidos por traço (ex: "Paciente-Um")

2. **calcular_tempo_tratamento(saude)**: Calcula o tempo em tics necessário para recuperar de uma saúde até 100 pontos
   - > 50 pontos: +1 ponto por tic
   - > 20 pontos: +1 ponto por cada 10 tics
   - > 10 pontos: +1 ponto por cada 100 tics (1 sol)
   - > 0 pontos: +1 ponto por cada 1000 tics (10 sols)

3. **calcular_degradacao(saude_inicial, tempo_espera)**: Calcula a saúde após um tempo de espera
   - > 50 pontos: -1 ponto por cada 1000 tics (10 sols)
   - > 20 pontos: -1 ponto por cada 100 tics (1 sol)
   - > 10 pontos: -1 ponto por cada 10 tics
   - > 0 pontos: -1 ponto por tic

### Comandos Implementados

#### Parte 1 - Câmaras Térmicas
- `# <nome>`: Adiciona uma câmara térmica com nome válido (dois nomes unidos por traço)
- `# relatorio global`: Mostra número de Aqualins e câmaras registados

#### Parte 2 - Registo de Aqualins
- `# <nome> <saude> <instante>`: Adiciona um Aqualin se:
  - Nome válido e único
  - Saúde entre 1 e 99 (não aceita mortos nem saudáveis)
  - Instante não decresce em relação ao anterior

#### Parte 3 - Tratamentos
- `# tratamentos`: Executa a calendarização de tratamentos
  - Usa fila FIFO para pacientes
  - Atribui câmara disponível mais cedo
  - Calcula degradação durante espera
  - Identifica mortes (saúde chega a 0 durante espera)
  - Calcula tempo de tratamento e alta
  - Mostra relatório de altas (primeira e última) e mortes

#### Parte 4 - Relatórios e Indicadores
- `# relatorio aqualins`: Lista todos os Aqualins com seu estado
- `# relatorio camaras`: Lista tratamentos por câmara
- `# indicador espera`: Tempo médio de espera (arredondado para baixo)
- `# indicador trabalho`: Total de tics·aqualin no sistema
- `# indicador salvos`: Número de Aqualins tratados com sucesso

## Como Usar

```bash
python3 aqualin_treatment.py < input.txt
```

O arquivo de entrada deve conter comandos, um por linha, começando com `#`. A aplicação termina quando encontra uma linha sem `#` ou com `#` sem palavras após.

## Exemplo de Entrada

```
# Camara-A
# Camara-B
# relatorio global
# Paciente-Um 60 0
# Paciente-Dois 30 0
# tratamentos
# relatorio aqualins
# indicador salvos
fim
```

## Exemplo de Saída

```
Aqualins: 0
Camaras: 2
Altas:
- primeira: Paciente-Um 60 0 0 40
- ultima: Paciente-Dois 30 0 0 259
Paciente-Um 60 0 0 alta:40
Paciente-Dois 30 0 0 alta:259
2
```

## Notas sobre a Implementação

1. **Ordem de tratamento**: Usa fila FIFO (primeiro a chegar, primeiro a ser tratado)
2. **Atribuição de câmaras**: Usa a câmara que fica disponível mais cedo; em caso de empate, usa ordem de inserção
3. **Degradação de saúde**: Calculada com precisão considerando as transições entre faixas de saúde
4. **Mortes**: Identificadas quando a saúde chega a 0 durante a espera
5. **Validação**: Comandos inválidos são ignorados, mas a aplicação continua processando

## Complexidade

- Adicionar câmara/Aqualin: O(n) no pior caso (verificação de duplicados)
- Tratamentos: O(n * m * log(m)) onde n = número de Aqualins, m = número de câmaras
- Relatórios: O(n) ou O(m) dependendo do tipo

## Melhorias Possíveis

1. Usar dicionários para validação de nomes únicos (O(1) em vez de O(n))
2. Usar heap para gerenciar disponibilidade de câmaras
3. Adicionar testes unitários para cada função
4. Adicionar validação mais rigorosa de entrada
