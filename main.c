#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdbool.h>

int max(int a, int b){
    return a>b?a:b;
}

typedef struct _data{
    int code;
    char city[1024];
    char street[1024];
    char neighborhood[1024];
    int postalCode;
    char streetType[1024];
}data;

typedef struct _node{
    data data;
    struct _node * pai;
    struct _node * esq;
    struct _node * dir;
    int h;
}tnode;

int altura(tnode * arv){
    int ret = -1;
    if (arv != NULL)
        ret = arv->h;
    
    return ret;
}

void avl_cria(tnode ** parv){
    *parv = NULL;
}

void RD(tnode **pparv, tnode ** pai) {
    tnode *y = *pparv;
    tnode *x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    y->pai = x;
    if (y->esq) y->esq->pai = y;
    *pparv = x;
    x->pai = *pai;
    y->h = 1 + max(altura(y->esq), altura(y->dir));
    x->h = 1 + max(altura(x->esq), y->h);
}

void RE(tnode **pparv, tnode ** pai) {
    tnode *x = *pparv;
    tnode *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    x->pai = y;
    if (x->dir) x->dir->pai = x;
    *pparv = y;
    y->pai = *pai;
    x->h = 1 + max(altura(x->esq), altura(x->dir));
    y->h = 1 + max(altura(y->dir), x->h);
}

void avl_rebalancear(tnode ** parv, tnode ** pai) {
    int fb; 
    int fbf;
    tnode *filho;

    fb = altura((*parv)->esq) - altura((*parv)->dir);
    if (fb == 2) {
        filho = (*parv)->esq;
        fbf = altura(filho->esq) - altura(filho->dir);
        if (fbf == -1) {
            RE(&(*parv)->esq, &*parv);
        }
        RD(parv, pai);
    } else if (fb == -2) {
        filho = (*parv)->dir;
        fbf = altura(filho->esq) - altura(filho->dir);
        if (fbf == 1) {
            RD(&(*parv)->dir, &*parv);
        }
        RE(parv, pai);
    }
}

void avl_insere(tnode ** parv,data reg, tnode ** pai){
    if (*parv == NULL){
        *parv = malloc(sizeof(tnode));
        (*parv)->data = reg;
        (*parv)->esq = NULL;
        (*parv)->dir = NULL;
        (*parv)->pai = *pai;
        (*parv)->h   = 0;
    }else if (reg.postalCode < (*parv)->data.postalCode){
        avl_insere(&(*parv)->esq,reg,&(*parv));
    }else{
        avl_insere(&(*parv)->dir,reg,&(*parv));
    }
    (*parv)->h = max(altura((*parv)->esq),altura((*parv)->dir)) + 1;
    avl_rebalancear(parv,pai);
}

void avl_destroi(tnode ** parv){
    if((*parv)->esq!=NULL){
        avl_destroi(&(*parv)->esq);
    }
    if(((*parv)->dir!=NULL)){
        avl_destroi(&(*parv)->dir);
    }
    free(&(**parv));
}

tnode * busca(tnode ** parv,int x){
    
    tnode * t = NULL;

    if(*parv!=NULL){

        if((*parv)->data.postalCode == x)
            return t = *parv;
        
        if((*parv)->data.postalCode > x)
            t = busca(&(*parv)->esq,x);
        
        if((*parv)->data.postalCode < x)
            t = busca(&(*parv)->dir,x);
    }
    return t;
}
void printData(tnode ** x){
    printf("------------------------\n");
    printf("Cidade: %s\n",(*x)->data.city);
    printf("Rua: %s\n",(*x)->data.street);
    printf("Bairro: %s\n",(*x)->data.neighborhood);
    printf("Código Postal: %d\n",(*x)->data.postalCode);
    printf("Tipo de Rua: %s",(*x)->data.streetType);
    printf("------------------------\n");
}

void buscaIntervaloCEP(tnode ** parv, int lower, int upper) {
    if (*parv == NULL) 
        return;
    
    if ((*parv)->data.postalCode >= lower && (*parv)->data.postalCode <= upper) 
        printData(&(*parv));
    
    if ((*parv)->data.postalCode >= lower) 
        buscaIntervaloCEP(&(*parv)->esq, lower, upper);
    
    if ((*parv)->data.postalCode <= upper) 
        buscaIntervaloCEP(&(*parv)->dir, lower, upper);
    
}

void menu(){
    printf("O que deseja fazer?\n");
    printf("1 - Buscar um unico CEP\n");
    printf("2 - Busca entre intervalos de CEP\n");
    printf("3 - Sair\n");
}

bool validarCep(tnode ** parv, const char* input) {
    if (input == NULL)
        return false;

    char* endptr;
    long num = strtol(input, &endptr, 10);

    if (*endptr == '\0' && num > 0 && num <= 99999999 && busca(parv,num)!=NULL)
        return true;
    else
        return false;
}

int main(void){
    
    tnode * pai = NULL;
    tnode * arv; 
    avl_cria(&arv); 
    
    FILE *arquivo = fopen("teste.csv","r");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    char linha[1024]; 
    while (fgets(linha, sizeof(linha), arquivo)) {
        data  data;
        if (sscanf(linha, "%d,%63[^,],%63[^,],%63[^,],%d,%63[^,]", &data.code, data.city, data.street, data.neighborhood, &data.postalCode, data.streetType) == 6) {
            avl_insere(&arv, data, &pai);
        }
    }

    fclose(arquivo);

    int n = -1;
    while(n!=3){

        char input1[1024];
        char input2[1024];
    
        menu();
        scanf("%d",&n);

        switch (n)
        {
        case 1:

            printf("\nDigite o CEP que deseja buscar: \n");
            scanf("%s",input1);

            if(validarCep(&arv,input1)){
                tnode * x = busca(&arv,atoi(input1));
                if(x==NULL){
                    printf("\nCEP não encontrado :( \n");
                }else{
                    printf("\nCEP Encontrado!!!\n");
                    printData(&x);
                }
            }else{
                printf("\nCEP Digitado é invalido\n");
                printf("Por favor, tente novamente!\n\n");
                break;
            }

            break;


        case 2: 

            printf("\nDigite o primeiro CEP que deseja buscar: \n");
            scanf("%s",input1);
            printf("Digite o segundo CEP que deseja buscar: \n");
            scanf("%s",input2);

            if(validarCep(&arv,input1) && validarCep(&arv,input2)){
                printf("CEPs encontrados nesse intervalo:\n");
                if(max(atoi(input1),atoi(input2))==atoi(input2)){
                    buscaIntervaloCEP(&arv, atoi(input1), atoi(input2));
                }else{
                    buscaIntervaloCEP(&arv, atoi(input2), atoi(input1));
                }
            }else{
                printf("\nCEP(s) Digitado(s) é(são) invalido(s)\n");
                printf("Por favor, tente novamente!\n\n");
                break;
            }

            break;
        
        default:
            break;
        }
    }
    avl_destroi(&arv);
    return EXIT_SUCCESS;
}
