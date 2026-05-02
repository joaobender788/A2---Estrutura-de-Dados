# Sistema de Gerenciamento de Eventos Críticos de uma Cidade Inteligente

## Descrição Geral

Este sistema foi desenvolvido em linguagem C para o gerenciamento em tempo real de eventos críticos urbanos (como acidentes de trânsito, falhas em semáforos, interrupções de energia, alagamentos e incêndios). O núcleo do sistema é estruturado utilizando uma **Árvore AVL**, o que garante alta eficiência nas operações de busca, inserção e atualização de dados, mesmo sob grande volume de registros.

A aplicação conta com um menu interativo e suporta operações de cadastro, atualização de status e severidade, remoção restrita (apenas a eventos resolvidos) e consultas avançadas por região, severidade ou intervalo de ID.

---

## Requisitos

* **Compilador C:** GCC (GNU Compiler Collection) ou compatível.
* **Sistema Operacional:** Compatível com Linux, macOS e Windows.

---

## Instruções de Compilação

Siga os passos abaixo para compilar o programa a partir do arquivo fonte (ex: `main.c`):

1. Abra o terminal ou prompt de comando na pasta onde o arquivo `main.c` está salvo.
2. Execute o seguinte comando para compilar o código e gerar o executável:

```bash
gcc -o gerenciador_eventos main.c -lm
```

> **Nota:** A flag `-lm` é utilizada para incluir a biblioteca de funções matemáticas padrão.

---

## Instruções de Execução

Após a compilação, o executável será gerado. Você pode executá-lo diretamente no terminal com o comando correspondente ao seu sistema operacional:

* **No Linux / macOS:**
    ```bash
    ./gerenciador_eventos
    ```

* **No Windows:**
    ```cmd
    gerenciador_eventos.exe
    ```

---

## Funcionalidades do Menu

O sistema exibe um menu textual com 8 opções principais para o usuário:

* **1. Cadastrar Evento:** Adiciona um novo evento à árvore (é verificado se o ID já existe e os dados passam por validações).
* **2. Remover Evento:** Remove um evento da árvore. O sistema só permite a remoção se o status do evento estiver como "Resolvido".
* **3. Buscar Evento por ID:** Busca o evento utilizando a propriedade de árvore binária de busca.
* **4. Consultas Avançadas:**
    * Listar eventos ativos por intervalo de severidade (de 1 a 5).
    * Gerar relatório por região, ordenado pelo ID do evento.
    * Listar eventos cujo ID esteja em um intervalo específico.
* **5. Atualizar Evento:**
    * Alterar o status de `ATIVO` para `RESOLVIDO`.
    * Atualizar o nível de severidade de um evento que ainda esteja `ATIVO`.
* **6. Métricas da Árvore:** Exibe a altura total, o número total de nós, a quantidade de eventos ativos, o fator de balanceamento médio e o total de rotações efetuadas.
* **7. Exibir Todos os Eventos (Em-Ordem):** Mostra todos os registros armazenados ordenados pela chave primária (ID).
* **8. Sair:** Libera toda a memória alocada dinamicamente e encerra a execução.
