# Solução do E-fólio B 2525/2526 - Gestão de Tratamentos Aqualin

## 📖 Sobre o Exercício

Este repositório contém a solução para o E-fólio B da disciplina **Introdução à Programação** (21173, 21090) do ano letivo 2025/2026.

O exercício pede a implementação de uma aplicação para gerir tratamentos de seres aquáticos fictícios chamados "Aqualins" em câmaras térmicas. O enunciado completo está disponível no arquivo `E-fólio B 2526.pdf`.

## 🚀 Como Executar

### Requisitos
- GCC (compilador C)
- Make (opcional, mas recomendado)

### Compilação
```bash
make
```

Ou manualmente:
```bash
gcc -Wall -Wextra -std=c99 -O2 -o aqualin_treatment aqualin_treatment.c
```

### Execução
```bash
./aqualin_treatment < arquivo_entrada.txt
```

## 📝 Formato dos Comandos

Todos os comandos começam com `#` e têm entre 1 a 3 palavras separadas por espaços.

### Comandos Disponíveis

#### Parte 1 - Gestão de Câmaras
```bash
# <nome-camara>          # Adiciona uma câmara (nome com formato: Palavra-Palavra)
# relatorio global       # Mostra total de Aqualins e Câmaras
```

#### Parte 2 - Registo de Pacientes
```bash
# <nome> <saude> <tic>   # Adiciona um Aqualin
                         # - nome: formato Palavra-Palavra
                         # - saude: 1-99 (0=morto, 100=saudável, ambos rejeitados)
                         # - tic: instante de chegada (não pode decrescer)
```

#### Parte 3 - Tratamentos
```bash
# tratamentos            # Executa calendarização e mostra relatório de altas
```

#### Parte 4 - Relatórios e Indicadores
```bash
# relatorio aqualins     # Lista todos os Aqualins e seus estados
# relatorio camaras      # Lista tratamentos por câmara
# indicador espera       # Tempo médio de espera
# indicador trabalho     # Total de tics·aqualin no sistema
# indicador salvos       # Número de Aqualins tratados com sucesso
```

### Encerramento
A aplicação encerra quando encontra:
- Uma linha que não começa com `#`
- Uma linha com `#` mas sem palavras após

## 💡 Regras de Tratamento

### Tempo de Recuperação (em câmara)
| Saúde Atual | Taxa de Recuperação |
|-------------|---------------------|
| > 50 pontos | +1 ponto por tic |
| > 20 pontos | +1 ponto por 10 tics |
| > 10 pontos | +1 ponto por 100 tics (1 sol) |
| > 0 pontos  | +1 ponto por 1000 tics (10 sols) |

### Degradação (em espera)
| Saúde Atual | Taxa de Degradação |
|-------------|---------------------|
| > 50 pontos | -1 ponto por 1000 tics (10 sols) |
| > 20 pontos | -1 ponto por 100 tics (1 sol) |
| > 10 pontos | -1 ponto por 10 tics |
| > 0 pontos  | -1 ponto por tic |

### Política de Atendimento
- **Fila**: FIFO (primeiro a chegar, primeiro a ser atendido)
- **Câmaras**: Usa a câmara disponível mais cedo; em empate, usa ordem de inserção
- **Morte**: Ocorre quando saúde chega a 0 durante espera

## 📊 Exemplos de Uso

### Exemplo 1: Caso Simples
```bash
# Camara-A
# Camara-B
# Patient-One 60 0
# Patient-Two 30 0
# tratamentos
fim
```

**Saída:**
```
Altas:
- primeira: Patient-One 60 0 0 40
- ultima: Patient-Two 30 0 0 259
```

### Exemplo 2: Com Mortes
```bash
# Single-Chamber
# Critical-Patient 2 0
# Healthy-Patient 80 0
# tratamentos
fim
```

**Saída:**
```
Altas:
- primeira: Healthy-Patient 80 0 0 20
Mortes: 1 2 2
```

## 🧪 Testes Incluídos

O repositório inclui vários arquivos de teste:

- `test_input.txt` - Teste básico com 3 câmaras e 4 pacientes
- `test_input2.txt` - Teste com espera e mortes
- `test_input3.txt` - Teste de validação (nomes inválidos, saúde inválida, etc.)
- `test_input4.txt` - Teste de cenários complexos de espera
- `test_input5.txt` - Teste abrangente com múltiplas execuções de tratamento

Execute todos os testes:
```bash
make test
```

Ou manualmente:
```bash
for test in test_input*.txt; do
    echo "=== Running $test ==="
    ./aqualin_treatment < "$test"
    echo ""
done
```

## 📚 Estrutura do Código

```
aqualin_treatment.c
├── Estruturas
│   ├── Camara - Representa uma câmara térmica
│   ├── Aqualin - Representa um paciente
│   └── Tratamento - Representa um tratamento realizado
├── Funções
│   ├── validar_nome() - Valida formato do nome
│   ├── calcular_tempo_tratamento() - Calcula tempo de recuperação
│   ├── calcular_degradacao() - Calcula degradação durante espera
│   ├── executar_tratamentos() - Calendariza e executa tratamentos
│   └── main() - Loop principal de processamento
```

## 📖 Documentação Adicional

Para mais detalhes sobre a implementação, consulte:
- `SOLUCAO_EFOLIO.md` - Explicação detalhada da solução
- `E-fólio B 2526.pdf` - Enunciado completo do exercício

## 🎓 Autor

Francisco Belo - Universidade Aberta - 2025/2026

## 📄 Licença

Este código é fornecido apenas para fins educacionais.
