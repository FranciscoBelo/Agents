# e-folio B: Respostas de apoio

Estas respostas estao escritas em texto ASCII e organizadas por pergunta. Adapte a formatacao conforme o modelo pedido pela avaliacao.

## 1) Plataforma de reservas

**a) Duas restricoes de integridade (1 entidade, 1 referencial)**
- Entidade: `Cliente(id_cliente, nome, email)` com `id_cliente` definido como `PRIMARY KEY` e `NOT NULL`, garantindo unicidade e existencia de identificador para cada cliente.
- Referencial: `Reserva(id_cliente)` como `FOREIGN KEY` para `Cliente(id_cliente)`, assegurando que cada reserva pertence a um cliente existente.

**b) Gatilho para impedir pagamento acima do valor_total** (PostgreSQL)
```sql
-- Bloqueia inserts/updates que excedam o valor_total da reserva associada
CREATE OR REPLACE FUNCTION trg_check_pagamento_valor()
RETURNS TRIGGER AS $$
DECLARE
    v_total numeric;
BEGIN
    SELECT r.valor_total INTO v_total
    FROM Reserva r
    WHERE r.id_reserva = NEW.id_reserva;

    IF v_total IS NULL THEN
        RAISE EXCEPTION 'Reserva % inexistente', NEW.id_reserva;
    END IF;

    IF NEW.valor > v_total THEN
        RAISE EXCEPTION 'Pagamento (%.2f) excede valor_total (%.2f) da reserva %',
            NEW.valor, v_total, NEW.id_reserva;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_pagamento_valor
BEFORE INSERT OR UPDATE ON Pagamento
FOR EACH ROW EXECUTE FUNCTION trg_check_pagamento_valor();
```
Explicacao: antes de gravar um pagamento, o trigger le `valor_total` da reserva associada. Se nao existir reserva, falha; se o pagamento for maior que `valor_total`, levanta excecao e cancela a operacao.

**c) Porque nao basta FK ou CHECK?**
- Uma chave estrangeira apenas garante correspondencia de chaves, nao valida magnitudes numericas.
- Um `CHECK` em `Pagamento` nao pode, no SQL padrao e no PostgreSQL, consultar outra tabela (`Reserva`). Logo nao consegue comparar `Pagamento.valor` com `Reserva.valor_total` sem logica procedural do gatilho.

## 2) Algebra relacional sobre inscricoes

**a) Expressao para nomes dos alunos inscritos em pelo menos uma UC em 2024**
\[
\pi_{nome}(\,Aluno \bowtie_{Aluno.n\_mec = Inscricao.n\_mec} \sigma_{ano = 2024}(Inscricao)\,)
\]
Justificacao: primeiro filtra inscricoes de 2024 (`sigma`), depois liga ao aluno pelo numero mecanografico (`bowtie`), e finalmente projeta apenas o nome (`pi`). O `bowtie` ja elimina alunos sem inscricao em 2024.

**b) Exemplo minimo de dados e resultados intermediarios**

_Tabelas de partida_
```
Aluno(n_mec, nome, curso)
(1, 'Ana', 'Eng. Inf')
(2, 'Bruno', 'Mat')
(3, 'Catarina', 'Hist')

UC(cod_uc, nome_uc)
('DB1', 'Bases de Dados')
('ALG', 'Algoritmos')

Inscricao(n_mec, cod_uc, ano)
(1, 'DB1', 2024)
(1, 'ALG', 2023)
(2, 'DB1', 2024)
```

_Passo 1 – selecao de inscricoes de 2024_
```
σ_{ano=2024}(Inscricao)
(1, 'DB1', 2024)
(2, 'DB1', 2024)
```

_Passo 2 – juncao com Aluno_
```
Aluno ⋈_{n_mec} resultado_anterior
(1, 'Ana', 'Eng. Inf', 'DB1', 2024)
(2, 'Bruno', 'Mat',     'DB1', 2024)
```

_Passo 3 – projecao do nome_
```
π_{nome}
'Ana'
'Bruno'
```
Resultado final: nomes dos alunos com pelo menos uma inscricao em 2024.

## 3) Projeto da base de dados de formacao

**a) Entidades, atributos e cardinalidades (texto resumido)**
- Pessoa(cc, nome, email, telefone) — entidade geral para qualquer individuo (identificada pelo cartao de cidadao).
- Formador(cc, area_principal) — especializacao de Pessoa (1:1 com Pessoa; participacao disjunta com Formando).
- Formando(cc, tipo_inscricao ['particular','externa'], entidade_externa NULL quando particular) — especializacao de Pessoa (1:1 com Pessoa).
- Curso(cod_curso, titulo, duracao_total_horas).
- Modulo(cod_curso, num_modulo, titulo, carga_horaria) — dependencia existencial do Curso (1 Curso : N Modulos), PK composta (cod_curso, num_modulo).
- Leciona(cc_formador, cod_curso, funcao, horas_lecionadas) — associacao M:N entre Formador e Curso com atributos proprios.
- Inscricao(cc_formando, cod_curso, data_inscricao, estado ['ativa','concluida','anulada']) — associacao M:N entre Formando e Curso.

Cardinalidades chave:
- Pessoa 1:1 Formador (opcional) e 1:1 Formando (opcional); disjuntas ou sobrepostas conforme politica, assumindo disjuntas neste contexto.
- Curso 1:N Modulo (modulo depende do curso e nao existe isolado).
- Formador M:N Curso via Leciona.
- Formando M:N Curso via Inscricao.

**b) Esboco do diagrama ER (descricoes textuais)**
- Generalizacao: Pessoa (cc) -> Formador (cc) e Formando (cc), especializacoes disjuntas.
- Relacao Curso 1..1 — 0..N Modulo; Modulo com atributo discriminador `num_modulo` sequencial por curso.
- Relacao Formador M..N Curso rotulada "Leciona" com atributos `funcao`, `horas_lecionadas`.
- Relacao Formando M..N Curso rotulada "Inscricao" com atributos `data_inscricao`, `estado`.

**c) Esquema relacional resultante e mapeamento**
```
Pessoa(cc PK, nome, email UNIQUE, telefone)
Formador(cc PK/FK Pessoa.cc, area_principal)
Formando(cc PK/FK Pessoa.cc, tipo_inscricao, entidade_externa)
Curso(cod_curso PK, titulo, duracao_total_horas)
Modulo(cod_curso FK -> Curso, num_modulo, titulo, carga_horaria,
       PRIMARY KEY (cod_curso, num_modulo))
Leciona(cc_formador FK -> Formador, cod_curso FK -> Curso,
        funcao, horas_lecionadas,
        PRIMARY KEY (cc_formador, cod_curso))
Inscricao(cc_formando FK -> Formando, cod_curso FK -> Curso,
          data_inscricao, estado,
          PRIMARY KEY (cc_formando, cod_curso, data_inscricao))
```
Justificacoes de mapeamento: especializacoes 1:1 disjuntas mapeadas em tabelas separadas com PK=FK para Pessoa; relacionamento 1:N (Curso-Modulo) mapeado com FK no lado N; relacionamentos M:N (Formador-Curso e Formando-Curso) mapeados com tabelas associativas contendo atributos proprios e PK composta.
