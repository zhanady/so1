#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdool.h>

struct funcionario {
    unsigned int id;
    char nome[256];
    char sexo;
    float salario;
}

bool idExiste(FILE * arquivo, unsigned int id) {
    struct funcionario func;
    rewind(arquivo);

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){
        if(func.id == id){
            return true;
        }
    }
    return false;
}

void inserirFuncionario(FILE * arquivo, unsigned int id, char nome[256], char sexo, float salario){
    struct funcionario func;
    
    if( id <= 0 ){
        printf("id invalido");
        return;
    }
    else{
        existe = idExiste(arquivo, id);
        if (existe == true){
            prinft("id já existe");
            return;
        }
    }

    func.id = id;
    strncpy(func.nome, nome, sizeof(func.nome) - 1);
    func.sexo = sexo;
    func.salario = salario;

    fseek(arquivo, 0, SEEK_END);
    fwrite(&func, sizeof(struct funcionario), 1, arquivo);
    prinft("funcionario inserido com sucesso!");
    return;
}

void removerFuncionario(FILE * arquivo, unsigned int id){
    struct funcionario func;
    bool encontrado = false;
    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){
        if(func.id == id){
            func.id = 0;
            fseek(arquivo, -sizeof(struct funcionario), arquivo);
            fwrite(&func, sizeof(struct funcionario), 1, arquivo);
            encontrado = true;
            prinft("funcionario removido");
        }
    }
    if(encontrado == false){
        printf("funcionario nao encontrado");
        return;   
    }
}

float mediaFuncionariosPorS(){
    return media;
}

void exportarDados() {

}

void compactarDados(){

}

int main(int argc, char* argv[]){
    if( argc != 3 ){
        printf("Uso indevido: main.exe <nome_arquivo> <nome_funcao>");
        return 1;
    }

    char* nome_arquivo = argv[1];
    char* funcao = argv[2];

    FILE * arquivo = fopen(nome_arquivo, "rb+");
    if (arquivo == NULL){
        FILE * arquivo = fopen(nome_arquivo, "wb+");
        if( arquivo == NULL){
            printf("erro ao criar/abrir arquivo \n");
        }
    }

    unsigned int id;
    char nome[256];
    char sexo;
    float salario;
    char nome_txt[256];
    char nome_saida[256];

    switch(funcao){
        case "inserir":
            prinft("Digite o ID: \n");
            scanf("%u \n", &id);
            prinft("Digite o nome: \n");
            scanf("%s \n", nome);
            prinft("Digite o sexo: \n");
            scanf("%c \n", &sexo)
            prinft("Digite o salario: \n");        
            scanf("%.2f \n", &salario);
            
            inserirFuncionario(arquivo, id, nome, sexo, salario);
            break;
        
        case "remover":
            prinft("Digite o ID: \n");
            scanf("%u \n", &id);
        
            removerFuncionario(arquivo, id);
            break;
        
        case "media":
            calcularMediaFuncionarios(arquivo);
            break;
        case "exportar":
            prinft("Digite o nome do arquivo de saida: \n");
            scanf("%s \n", &nome_saida);
            
            exportarDados(arquivo, nome_saida);
            break;
        case "compactar":
            compactarDados(*arquivo, nome_arquivo);
            break;
    }
}

