#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Struct para representar a data e hora do evento crítico
typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
} Timestamp;

// Enum para representar o status atual do evento (Ativo ou Resolvido)
typedef enum { ATIVO = 0, RESOLVIDO = 1 } EventStatus;

// Estrutura do nó da Árvore AVL
typedef struct No {
    int id; // Chave primária de ordenação
    char tipo[50]; // Tipo do evento
    int severidade; // Nível de 1 a 5
    Timestamp timestamp; // Data e hora do registro
    char regiao[50]; // Região da cidade
    EventStatus status; // Status do evento
    int altura; // Altura do nó para cálculo do fator de balanceamento
    struct No *esquerdo; // Ponteiro para o filho esquerdo
    struct No *direito; // Ponteiro para o filho direito
} NoAVL;

// Contador global para registrar a quantidade de rotações realizadas
int total_rotacoes = 0;

// Retorna a altura de um determinado nó (ou 0 se o nó for nulo)
int altura(NoAVL *N) {
    if (N == NULL)
        return 0;
    return N->altura;
}

// Retorna o maior entre dois números inteiros
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Aloca e inicializa um novo nó na memória
NoAVL* novoNo(int id, char* tipo, int severidade, Timestamp ts, char* regiao, EventStatus status) {
    NoAVL* no = (NoAVL*)malloc(sizeof(NoAVL));
    
    no->id = id;
    strcpy(no->tipo, tipo);
    no->severidade = severidade;
    no->timestamp = ts;
    strcpy(no->regiao, regiao);
    no->status = status;
    no->altura = 1; // Novo nó sempre começa com altura 1
    no->esquerdo = NULL;
    no->direito = NULL;
    
    return no;
}

// Realiza uma rotação simples à direita
NoAVL* rotacaoDireita(NoAVL *y) {
    NoAVL *x = y->esquerdo;
    NoAVL *T2 = x->direito;

    // Executa a rotação
    x->direito = y;
    y->esquerdo = T2;

    // Atualiza as alturas
    y->altura = max(altura(y->esquerdo), altura(y->direito)) + 1;
    x->altura = max(altura(x->esquerdo), altura(x->direito)) + 1;

    // Retorna a nova raiz da subárvore
    return x;
}

// Realiza uma rotação simples à esquerda
NoAVL* rotacaoEsquerda(NoAVL *x) {
    NoAVL *y = x->direito;
    NoAVL *T2 = y->esquerdo;

    // Executa a rotação
    y->esquerdo = x;
    x->direito = T2;

    // Atualiza as alturas
    x->altura = max(altura(x->esquerdo), altura(x->direito)) + 1;
    y->altura = max(altura(y->esquerdo), altura(y->direito)) + 1;

    // Retorna a nova raiz da subárvore
    return y;
}

// Calcula o fator de balanceamento de um nó
int getFatorBalanceamento(NoAVL *N) {
    if (N == NULL)
        return 0;
    return altura(N->esquerdo) - altura(N->direito);
}

// Insere um novo evento na árvore e mantém o balanceamento AVL
NoAVL* inserir(NoAVL* no, int id, char* tipo, int severidade, Timestamp ts, char* regiao, EventStatus status, int *rotacoes) {
    // 1. Executa a inserção normal de uma BST
    if (no == NULL)
        return (novoNo(id, tipo, severidade, ts, regiao, status));

    if (id < no->id)
        no->esquerdo = inserir(no->esquerdo, id, tipo, severidade, ts, regiao, status, rotacoes);
    else if (id > no->id)
        no->direito = inserir(no->direito, id, tipo, severidade, ts, regiao, status, rotacoes);
    else
        return no; // Chaves duplicadas não são permitidas

    // 2. Atualiza a altura do nó ancestral
    no->altura = 1 + max(altura(no->esquerdo), altura(no->direito));

    // 3. Obtém o fator de balanceamento para checar se está desbalanceado
    int bal = getFatorBalanceamento(no);

    // Casos de desbalanceamento:

    // Caso Esquerda-Esquerda
    if (bal > 1 && id < no->esquerdo->id) {
        (*rotacoes)++;
        return rotacaoDireita(no);
    }

    // Caso Direita-Direita
    if (bal < -1 && id > no->direito->id) {
        (*rotacoes)++;
        return rotacaoEsquerda(no);
    }

    // Caso Esquerda-Direita
    if (bal > 1 && id > no->esquerdo->id) {
        (*rotacoes)++;
        no->esquerdo = rotacaoEsquerda(no->esquerdo);
        return rotacaoDireita(no);
    }

    // Caso Direita-Esquerda
    if (bal < -1 && id < no->direito->id) {
        (*rotacoes)++;
        no->direito = rotacaoDireita(no->direito);
        return rotacaoEsquerda(no);
    }

    return no;
}

// Função auxiliar para encontrar o nó com o menor valor (usado na remoção)
NoAVL* minValueNode(NoAVL* node) {
    NoAVL* current = node;
    while (current->esquerdo != NULL)
        current = current->esquerdo;
    return current;
}

// Remove um evento da árvore
// *removido retorna: 1 para sucesso, 0 para não encontrado, -1 para evento ativo (não permitido).
NoAVL* remover(NoAVL* root, int id, int *rotacoes, int *removido) {
    // 1. Busca padrão da BST
    if (root == NULL) {
        *removido = 0;
        return root;
    }

    if (id < root->id)
        root->esquerdo = remover(root->esquerdo, id, rotacoes, removido);
    else if (id > root->id)
        root->direito = remover(root->direito, id, rotacoes, removido);
    else {
        // Validação de negócio solicitada: apenas resolve resolvidos
        if (root->status != RESOLVIDO) {
            *removido = -1; // Status ATIVO impede remoção
            return root;
        }
        *removido = 1;

        // Nó com 0 ou 1 filho
        if ((root->esquerdo == NULL) || (root->direito == NULL)) {
            NoAVL *temp = root->esquerdo ? root->esquerdo : root->direito;

            if (temp == NULL) { // Sem filhos
                temp = root;
                root = NULL;
            } else { // Um filho
                *root = *temp;
            }
            free(temp);
        } else {
            // Nó com dois filhos: pega o sucessor em-ordem (menor da subárvore direita)
            NoAVL* temp = minValueNode(root->direito);

            // Copia os dados do sucessor para o nó atual
            root->id = temp->id;
            strcpy(root->tipo, temp->tipo);
            root->severidade = temp->severidade;
            root->timestamp = temp->timestamp;
            strcpy(root->regiao, temp->regiao);
            root->status = temp->status;

            // Remove o sucessor em-ordem
            root->direito = remover(root->direito, temp->id, rotacoes, removido);
        }
    }

    // Se a árvore tinha apenas 1 nó
    if (root == NULL)
        return root;

    // 2. Atualiza a altura do nó atual
    root->altura = 1 + max(altura(root->esquerdo), altura(root->direito));

    // 3. Rebalanceia
    int balance = getFatorBalanceamento(root);

    if (balance > 1 && getFatorBalanceamento(root->esquerdo) >= 0) {
        (*rotacoes)++;
        return rotacaoDireita(root);
    }
    if (balance > 1 && getFatorBalanceamento(root->esquerdo) < 0) {
        (*rotacoes)++;
        root->esquerdo = rotacaoEsquerda(root->esquerdo);
        return rotacaoDireita(root);
    }
    if (balance < -1 && getFatorBalanceamento(root->direito) <= 0) {
        (*rotacoes)++;
        return rotacaoEsquerda(root);
    }
    if (balance < -1 && getFatorBalanceamento(root->direito) > 0) {
        (*rotacoes)++;
        root->direito = rotacaoDireita(root->direito);
        return rotacaoEsquerda(root);
    }

    return root;
}

// Busca um nó por ID
NoAVL* buscar(NoAVL* root, int id) {
    if (root == NULL || root->id == id)
        return root;

    if (root->id < id)
        return buscar(root->direito, id);

    return buscar(root->esquerdo, id);
}

// Imprime os dados formatados de um evento
void imprimirEvento(NoAVL* no) {
    if (no == NULL) return;
    printf("\n---------------------------------------------\n");
    printf(" ID: %d\n", no->id);
    printf(" Tipo: %s\n", no->tipo);
    printf(" Severidade: %d\n", no->severidade);
    printf(" Data/Hora: %02d/%02d/%4d %02d:%02d\n", no->timestamp.dia, no->timestamp.mes, no->timestamp.ano, no->timestamp.hora, no->timestamp.minuto);
    printf(" Regiao: %s\n", no->regiao);
    printf(" Status: %s\n", no->status == ATIVO ? "ATIVO" : "RESOLVIDO");
    printf("---------------------------------------------\n");
}

// Lista eventos ativos por severidade mínima e máxima
void listarEventosPorSeveridade(NoAVL* root, int minSev, int maxSev, int *found) {
    if (root == NULL) return;
    listarEventosPorSeveridade(root->esquerdo, minSev, maxSev, found);
    
    if (root->status == ATIVO && root->severidade >= minSev && root->severidade <= maxSev) {
        imprimirEvento(root);
        (*found)++;
    }
    
    listarEventosPorSeveridade(root->direito, minSev, maxSev, found);
}

// Retorna eventos ativos pertencentes a uma região específica
void relatorioPorRegiao(NoAVL* root, const char* regiao, int *found) {
    if (root == NULL) return;
    relatorioPorRegiao(root->esquerdo, regiao, found);
    
    if (root->status == ATIVO && strcmp(root->regiao, regiao) == 0) {
        imprimirEvento(root);
        (*found)++;
    }
    
    relatorioPorRegiao(root->direito, regiao, found);
}

// Busca eventos em um intervalo numérico de ID
void buscarPorIntervaloID(NoAVL* root, int minID, int maxID, int *found) {
    if (root == NULL) return;
    
    if (root->id > minID) {
        buscarPorIntervaloID(root->esquerdo, minID, maxID, found);
    }
    
    if (root->id >= minID && root->id <= maxID) {
        imprimirEvento(root);
        (*found)++;
    }
    
    if (root->id < maxID) {
        buscarPorIntervaloID(root->direito, minID, maxID, found);
    }
}

// Exibe a árvore utilizando percurso em-ordem (valores ordenados)
void emOrdem(NoAVL* root) {
    if (root != NULL) {
        emOrdem(root->esquerdo);
        imprimirEvento(root);
        emOrdem(root->direito);
    }
}

// Conta a quantidade de nós presentes na árvore
int contarNos(NoAVL* root) {
    if (root == NULL) return 0;
    return 1 + contarNos(root->esquerdo) + contarNos(root->direito);
}

// Conta a quantidade de nós que possuem status ATIVO
int contarAtivos(NoAVL* root) {
    if (root == NULL) return 0;
    int count = (root->status == ATIVO) ? 1 : 0;
    return count + contarAtivos(root->esquerdo) + contarAtivos(root->direito);
}

// Calcula o somatório dos fatores de balanceamento para obter uma média
void calcularFatores(NoAVL* root, double *soma, int *count) {
    if (root == NULL) return;
    *soma += abs(getFatorBalanceamento(root));
    (*count)++;
    calcularFatores(root->esquerdo, soma, count);
    calcularFatores(root->direito, soma, count);
}

// Libera toda a memória da árvore recursivamente
void liberarArvore(NoAVL* root) {
    if (root != NULL) {
        liberarArvore(root->esquerdo);
        liberarArvore(root->direito);
        free(root);
    }
}

int main() {
    NoAVL* raiz = NULL;
    int rotacoes = 0;
    int opcao = 0;

    // Loop do menu textual interativo
    do {
        printf("\n=============================================\n");
        printf(" SISTEMA DE GERENCIAMENTO DE EVENTOS CRITICOS\n");
        printf("=============================================\n");
        printf("1. Cadastrar Evento\n");
        printf("2. Remover Evento\n");
        printf("3. Buscar Evento por ID\n");
        printf("4. Consultas Avancadas\n");
        printf("5. Atualizar Evento\n");
        printf("6. Metricas da Arvore\n");
        printf("7. Exibir Todos os Eventos (Em-Ordem)\n");
        printf("8. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                int id, severidade, status_escolha;
                char tipo[50], regiao[50];
                Timestamp ts;

                printf("ID do Evento: ");
                scanf("%d", &id);
                
                // Validação de chave primária repetida
                if (buscar(raiz, id) != NULL) {
                    printf("Erro: ID ja existente na arvore!\n");
                    break;
                }

                printf("Tipo do Evento (ex: Acidente, Incendio): ");
                scanf(" %[^\n]", tipo);

                printf("Nivel de Severidade (1 a 5): ");
                scanf("%d", &severidade);
                
                // Validação da regra de negócio de severidade
                if (severidade < 1 || severidade > 5) {
                    printf("Erro: Severidade invalida.\n");
                    break;
                }

                printf("Data e Hora (DD MM YYYY HH MM): ");
                scanf("%d %d %d %d %d", &ts.dia, &ts.mes, &ts.ano, &ts.hora, &ts.minuto);

                printf("Regiao da cidade: ");
                scanf(" %[^\n]", regiao);

                printf("Status (0 - Ativo, 1 - Resolvido): ");
                scanf("%d", &status_escolha);
                EventStatus status = (status_escolha == 1) ? RESOLVIDO : ATIVO;

                raiz = inserir(raiz, id, tipo, severidade, ts, regiao, status, &rotacoes);
                total_rotacoes += rotacoes;
                rotacoes = 0; // Reinicializa o contador local
                printf("Evento cadastrado com sucesso!\n");
                break;
            }
            case 2: {
                int id;
                printf("ID do evento a remover: ");
                scanf("%d", &id);
                
                int removido = 0;
                raiz = remover(raiz, id, &rotacoes, &removido);
                
                if (removido == 1) {
                    printf("Evento removido com sucesso!\n");
                } else if (removido == -1) {
                    printf("Erro: O evento esta ATIVO, remocao nao permitida.\n");
                } else {
                    printf("Erro: Evento nao encontrado.\n");
                }
                
                total_rotacoes += rotacoes;
                rotacoes = 0;
                break;
            }
            case 3: {
                int id;
                printf("ID a buscar: ");
                scanf("%d", &id);
                NoAVL* res = buscar(raiz, id);
                if (res != NULL) {
                    imprimirEvento(res);
                } else {
                    printf("Evento nao encontrado.\n");
                }
                break;
            }
            case 4: {
                int subopcao;
                printf("\nConsultas Avancadas\n");
                printf("1. Listar eventos ativos por intervalo de severidade\n");
                printf("2. Relatorio por regiao\n");
                printf("3. Buscar por intervalo de ID\n");
                printf("Escolha: ");
                scanf("%d", &subopcao);

                if (subopcao == 1) {
                    int minSev, maxSev;
                    printf("Severidade minima (1-5): ");
                    scanf("%d", &minSev);
                    printf("Severidade maxima (1-5): ");
                    scanf("%d", &maxSev);
                    int count = 0;
                    listarEventosPorSeveridade(raiz, minSev, maxSev, &count);
                    if (count == 0) printf("Nenhum evento encontrado.\n");
                } else if (subopcao == 2) {
                    char regiao[50];
                    printf("Regiao: ");
                    scanf(" %[^\n]", regiao);
                    int count = 0;
                    relatorioPorRegiao(raiz, regiao, &count);
                    if (count == 0) printf("Nenhum evento encontrado nesta regiao.\n");
                } else if (subopcao == 3) {
                    int minID, maxID;
                    printf("ID minimo: ");
                    scanf("%d", &minID);
                    printf("ID maximo: ");
                    scanf("%d", &maxID);
                    int count = 0;
                    buscarPorIntervaloID(raiz, minID, maxID, &count);
                    if (count == 0) printf("Nenhum evento encontrado neste intervalo de ID.\n");
                } else {
                    printf("Opcao invalida.\n");
                }
                break;
            }
            case 5: {
                int subopcao;
                printf("\nAtualizar Evento\n");
                printf("1. Alterar status (Ativo -> Resolvido)\n");
                printf("2. Atualizar severidade\n");
                printf("Escolha: ");
                scanf("%d", &subopcao);

                if (subopcao == 1) {
                    int id;
                    printf("ID do evento: ");
                    scanf("%d", &id);
                    NoAVL* res = buscar(raiz, id);
                    if (res != NULL) {
                        res->status = RESOLVIDO;
                        printf("Status alterado para RESOLVIDO com sucesso!\n");
                    } else {
                        printf("Evento nao encontrado.\n");
                    }
                } else if (subopcao == 2) {
                    int id, novaSev;
                    printf("ID do evento: ");
                    scanf("%d", &id);
                    NoAVL* res = buscar(raiz, id);
                    if (res != NULL) {
                        // Verifica se o status está ativo para poder modificar
                        if (res->status == ATIVO) {
                            printf("Nova severidade (1-5): ");
                            scanf("%d", &novaSev);
                            if (novaSev >= 1 && novaSev <= 5) {
                                res->severidade = novaSev;
                                printf("Severidade atualizada com sucesso!\n");
                            } else {
                                printf("Erro: Severidade deve estar entre 1 e 5.\n");
                            }
                        } else {
                            printf("Erro: O evento nao esta ATIVO.\n");
                        }
                    } else {
                        printf("Evento nao encontrado.\n");
                    }
                }
                break;
            }
            case 6: {
                int totalNos = contarNos(raiz);
                int totalAtivos = contarAtivos(raiz);
                double somaFB = 0;
                int countFB = 0;
                calcularFatores(raiz, &somaFB, &countFB);
                double fbMedio = countFB > 0 ? (somaFB / countFB) : 0.0;

                printf("\n--- Metricas da Arvore ---\n");
                printf("Altura total da arvore: %d\n", altura(raiz));
                printf("Numero total de nos: %d\n", totalNos);
                printf("Numero de eventos ativos: %d\n", totalAtivos);
                printf("Fator de balanceamento medio: %.2f\n", fbMedio);
                printf("Quantidade total de rotacoes: %d\n", total_rotacoes);
                break;
            }
            case 7: {
                printf("\nExibindo todos os Eventos (Em-Ordem)\n");
                emOrdem(raiz);
                break;
            }
            case 8:
                printf("Encerrando o sistema...\n");
                liberarArvore(raiz);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 8);

    return 0;
}
