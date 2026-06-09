#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


void removerFuncionario(FILE * arquivo, unsigned int id){ //anota isso
    struct funcionario func; //ANOTAR A PARTE PQ VAI EM TODOS
    rewind(arquivo); // TAMBEM VAI EM TDOSO EXCETO INSERIR
    bool encontrado;

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){ // ANOTAR A PARTE -> *loop
        if(func.id == id){
            func.id = 0; 

            fseek(arquivo, -sizeof(struct funcionario), SEEK_CUR); // anotar 
            fwrite(&func, sizeof(struct funcionario), 1, arquivo); // anotar

            encontrado = true;
            printf("Funcionario com id %u removido. \n", id);
            break;
        }
    }
    if(!encontrado){
        printf("Funcionario com id %u nao encontrado. \n", id);
    }
}

void inserirFuncionario(FILE * arquivo, unsigned int id, char nome[256], char sexo, float salario) { //anotar  
    strncpy(func.nome, nome, sizeof(func.nome) - 1);  //anotar
    func.nome[sizeof(func.nome) - 1] = '\0'; //anotar
    //RECEBER ENTRADA E PASSAR PRA ESTRUTURA

    fseek(arquivo, 0, SEEK_END); //anotar
    fwrite(&func, sizeof(struct funcionario), 1, arquivo); //RESUMIR A ESCREVER
}

void exportarFuncionarios(FILE * arquivo, char* nome_arquivo_saida){
    FILE * arquivo_saida = fopen(nome_arquivo_saida, "w"); //// criar arquivo 
    if(arquivo_saida == NULL){                               // criar arquivo               
        printf("erro ao criar o arquivo de texto \n");       // criar arquivo       
        return;                                              // criar arquivo
    }

    while(fread(&func, sizeof(struct funcionario), 1, arquivo) == 1){ //ja ta definido ocmo loop ai tu so escreve loop
        if(func.id != 0){                                             //                  
            fprintf(arquivo_saida, "ID: %u", func.id);                //  
        }                                                             //       
    }                                                                 //
    fclose(arquivo_saida); // anotar
}

void compactarArquivo(FILE ** arquivo, const char* nome_arquivo){ //DESTACAR QUE PASSAMOS O ARQUIVO COM *
    FILE * arquivo_temp = fopen("temp.dat", "wb");  //ANOTA

    rewind(*arquivo); //rewind COM *

    while(fread(&func, sizeof(struct funcionario), 1, *arquivo) == 1){      // RESUMAR A LOOP -> ESCREVER
        if(func.id != 0){                                                   //  
            fwrite(&func, sizeof(struct funcionario), 1, arquivo_temp);     //
        }                                                                   //
    }

    fclose(*arquivo); //ANOTA
    fclose(arquivo_temp); //ANOTA

    remove(nome_arquivo); //ANOTA
    rename("temp.dat", nome_arquivo); //ANOTA

    *arquivo = fopen(nome_arquivo, "rb+"); //ANOTA
}

int main(int argc, char* argv[]){ // ANOTA
    if(argc != 3){                // anota 
    }                             //    

    char* nome_arquivo = argv[1]; //anota
    char* func = argv[2]; //anota

    FILE *arquivo = fopen(nome_arquivo, "rb+");                             //DEFINIR CRIAR DE ARQUIVO ETC
    if (arquivo == NULL) {
        arquivo = fopen(nome_arquivo, "wb+");
        if (arquivo == NULL) {
            fprintf(stderr, "Erro ao abrir/criar o arquivo base.\n");
            return 1;
        }
    }

    //definir dados quando ocorrerem chamadas tipo: char nome_txt[256];
     
    fclose(arquivo); //FECHAR o arquivo
}

//no lugar que definir na folha, botar os diferentes niveis de acesso (w, wb+, rb+)
//SLIDE 11
//SLIDE 14
//SLIDE 16
//SLIDE 17