#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct funcionario {
    unsigned int id;
    char nome[256];
    char sexo;
    float salario;
}; 

bool idExiste(FILE * arquivo, unsigned int id){
    struct funcionario func;
    rewind(arquivo);

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){
        if(func.id == id){
            return true;
        }
    }
    return false;
}

void removerFuncionario(FILE * arquivo, unsigned int id){
    struct funcionario func;
    rewind(arquivo);
    bool encontrado;

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){
        if(func.id == id){
            func.id = 0; 

            fseek(arquivo, -sizeof(struct funcionario), SEEK_CUR);
            fwrite(&func, sizeof(struct funcionario), 1, arquivo);

            encontrado = true;
            printf("Funcionario com id %u removido. \n", id);
            break;
        }
    }
    if(!encontrado){
        printf("Funcionario com id %u nao encontrado. \n", id);
    }
}

void inserirFuncionario(FILE * arquivo, unsigned int id, char nome[256], char sexo, float salario) {
    if( id <= 0 ){
        printf("Erro: ID deve ser um numero positivo.\n");
        return;
    }    
    if(idExiste(arquivo, id)){
        printf("Erro: ID %u ja existe. Escolha outro ID.\n", id);
        return;
    }

    struct funcionario func;
    func.id = id;
    strncpy(func.nome, nome, sizeof(func.nome) - 1);
    func.nome[sizeof(func.nome) - 1] = '\0';
    func.sexo = sexo;
    func.salario = salario;

    fseek(arquivo, 0, SEEK_END);
    fwrite(&func, sizeof(struct funcionario), 1, arquivo);
    printf("funcionario inserido com sucesso!");
}

float calcularMediaFuncionarios(FILE * arquivo, unsigned int id){
    struct funcionario func;
    rewind(arquivo);

    float somaM = 0.0, somaF = 0.0;
    int qtdM = 0, qtdF = 0;

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){
        if(func.id != 0){
            if(func.sexo == 'M' || func.sexo == 'm'){
                somaM += func.salario;
                qtdM++;
            }
            else if (func.sexo == 'F' || func.sexo == 'f'){
                somaF += func.salario;
                qtdF++;

            }
        }
    }
    if(qtdM > 0){
        printf("Media salarial masculina: %.2f\n", somaM/qtdM);
    }
    if(qtdF > 0){
        printf("Media salarial feminina: %.2f\n", somaF/qtdF);
    }
}

void exportarFuncionarios(FILE * arquivo, char* nome_arquivo_saida){
    FILE * arquivo_saida = fopen(nome_arquivo_saida, "w");
    if(arquivo_saida == NULL){
        printf("erro ao criar o arquivo de texto \n");
        return;
    }

    struct funcionario func;
    rewind(arquivo);

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){
        if(func.id != 0){
            fprintf(arquivo_saida, "ID: %u | Nome: %s | Sexo: %c | Salario: %.2f\n", func.id, func.nome, func.sexo, func.salario);
        }
    }
    fclose(arquivo_saida);
    printf("Dados exportados com sucesso");
}

void compactarArquivo(FILE ** arquivo, const char* nome_arquivo){
    FILE * arquivo_temp = fopen("temp.dat", "wb"); 

    if(arquivo_temp == NULL){
        printf("Erro ao criar arquivo temporario.\n");
        return;
    }

    struct funcionario func;
    rewind(*arquivo);

    while(fread(&func, sizeof(struct funcionario), 1, *arquivo) == 1){
        if(func.id != 0){
            fwrite(&func, sizeof(struct funcionario), 1, arquivo_temp);
        }
    }

    fclose(*arquivo);
    fclose(arquivo_temp);

    remove(nome_arquivo);
    rename("temp.dat", nome_arquivo);

    *arquivo = fopen(nome_arquivo, "rb+");
    printf("base de dados compactada com sucesso! \n");
}

int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "Uso: %s <arquivo> <func>\n", argv[0]);
        return 1;
    }

    char* nome_arquivo = argv[1];
    char* func = argv[2]; 

    FILE *arquivo = fopen(nome_arquivo, "rb+"); 
    if (arquivo == NULL) {
        arquivo = fopen(nome_arquivo, "wb+");
        if (arquivo == NULL) {
            fprintf(stderr, "Erro ao abrir/criar o arquivo base.\n");
            return 1;
        }
    }

    unsigned int id;
    char nome[256];
    char sexo;
    float salario;
    char nome_txt[256];
    char nome_saida[256];

    switch(func[0]){
        case 'i': 
            printf("Digite o id ");
            scanf("%u", &id);
            printf("Digite o nome ");
            scanf("%s", nome);
            printf("Digite o sexo ");
            scanf(" %c", &sexo);
            printf("Digite o salario ");
            scanf("%f", &salario);

            inserirFuncionario(arquivo, id, nome, sexo, salario);
            break;
        case 'r':
            printf("Digite o id do funcionario a ser removido: ");
            scanf("%u", &id);
            removerFuncionario(arquivo, id);
            break;
        case 'm':
            calcularMediaFuncionarios(arquivo, 1);
            break;
        case 'e':
            printf("Digite o nome do arquivo de saida (ex: saídas.txt): ");
            scanf(" %s", nome_saida);
            exportarFuncionarios(arquivo, nome_saida);
            break;
        case 'c':
            printf("Digite o nome do arquivo de saída: ");
            scanf("%s", nome_saida);
            compactarArquivo(&arquivo, nome_saida);
            break;
        default:
            fprintf(stderr, "Funcao desconhecida: %s\n", func);
    }

    fclose(arquivo);

    return 0;
}