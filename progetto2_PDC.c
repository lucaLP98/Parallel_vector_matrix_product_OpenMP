/*************************************************************************************
 Studente: Pastore Luca
 Matricola: N97000431

 Anno Accademico: 2023/24
 Corso: Parallel And Distributed Computing
 Prof.: Laccetti Giuliano, Mele Valeria
 Corso di Laurea Magistrale in Informatica
 Università degli studi Federico II - Napoli

 Traccia:
	Sviluppare un algoritmo per il calcolo del prodotto matrice-vettore, in ambiente di 
    calcolo parallelo su architettura MIMD a memoria condivisa, che utilizzi la libreria OpenMp.
**************************************************************************************/

//SEZIONE DICHIARAZIONE HEADER FILE
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*
INPUT DA TERMINALE
l'input è costituito da due valori passati via terminale
	- argv[1] = numero di righe della matrice
	- argv[2] = numero di colonne della matrice e di elementi del vettore
N.B.: argv[0] contiene il nome del programma
	- argc = contiene il numero di parametri di input passati da terminale
*/

//SEZIONE DICHIARAZIONE PROTOTIPI FUNZIONI
int check_input(int argc, char **argv);
int **create_matrix(int n, int m);
int *create_array(int m);
int *matxvet(int n, int m, int *x, int **A);
void print_array(int *x, int n, char *arrayName);

//INIZIO MAIN PROGRAMM
int main(int argc, char** argv){
    int **A, //Matrice n x m
        *x, //Vettore di n elementi
        *ris; //variabile che conterra il risultato del prodotto matrice vettore

    int n, //numero di righe della matrice
        m, //numero di colonne della matrice e di lementi del vettore
        checkInput, //variabile che indicherà la correttezza dei valori passati in input
        i, j;

    checkInput = check_input(argc,argv); //Verifica della correttezza dei parametri passati in input
    if(checkInput != 0) exit(checkInput);
    n = atoi(argv[1]); //Recupero il numero di righe della matrice
    m = atoi(argv[2]); //Recupero il numero di colonne della matrice e la dimensione del vettore

    A = create_matrix(n, m); //creazione di una matrice di interi di dimensioni n x m
    x = create_array(m); //creazione di un vettore di interi di dimensioni m

    //Calcolo del prodotto matrice vettore
    ris = matxvet(n,m,x,A); 

    //Stampa del risultato
    print_array(ris, n, "ris");

    return 0;
} //FINE MAIN PROGRAMM

//SEZIONE DEFINIZIONE FUNZIONI E PROCEDURE

/*
    Funzione utilizzata per verificare la correttezza dei parametri passati in input.
    - argc = numero di valori in input
    - argv = array conenente i valori passati da riga di comando, di tipo char
    - output:
        - 1 se il numero di parametri passati in input non è corretto 
        - 2 se il numero di righe della matrice è minore o uguale di 0
        - 3 se il numero di colonne della matrice (ed elementi dell'array) è minore o uguali di 0
*/
int check_input(int argc, char **argv){
    if(argc!=3){ //Controllo correttezza numero di parametri passati in input
        printf("\n Inserire i 2 argomenti richiesti: \n1. Numero di righe della matrice\n2. Numero di colonne della matrice (e quindi dimensione vettore)\n");
        return 1;
    }

    if(atoi(argv[1]) <= 0){ //Controllo correttezza numero di righe della matrice
        printf("\n Il numero di righe della matrice deve essere un valore maggiore di 0\n");
        return 2;
    }

    if(atoi(argv[2]) <= 0){ //Controllo correttezza numero di colonne matrice (ed elementi vettore)
        printf("\n Il numero di colonne della matrice (e quindi dimensione vettore) deve essere un valore maggiore di 0\n");
        return 3;
    }

    return 0; //valori di input corretti
}

/*
    Funzione utilizzata per generare una matrice di dimensione n x m
    - n = numero di righe della matrice
    - m = numero di colonne della matrice
    - output: matrice n x m di valori interi
*/
int **create_matrix(int n, int m){
    int **A; //Matrice n x m
    int i, j;

    /********** inizio fase di allocazione memoria per matrice **********/
    A = (int**) malloc (n*sizeof(int*)); //Allocazione righe della matrice
    for(i=0;i<n;i++)
        A[i] = (int*) malloc (m*sizeof(int)); //Allocazione colonne della matrice
    /********** fine fase di allocazione memoria per matrice **********/

    /********** inizio fase di inizializzazione valori per matrice **********/
    for(i=0;i<n;i++)
        for(j=0;j<m;j++)
            A[i][j] = (i*m) + j + 1; //Inizializzazione valori matrice da 1 a NxM
    /********** fine fase di inizializzazione valori per matrice **********/

    return A;
}

/*
    Funzione utilizzata per generare un vettore di dimensione m
    - m = numero di elementi del vettore
    - output: vettore di m elementi
*/
int *create_array(int m){
    int *x; //vettore di dimensioni m
    int i;

    x = (int*) malloc (m*sizeof(int)); //Allocazione memoria per vettore

    /********** inizio fase di inizializzazione valori per vettore **********/
    for(i=0; i<m; i++)
        x[i] = i+1; //Inizializzazione valori vettore da 1 a M
    /********** fine fase di inizializzazione valori per vettore **********/

    return x;
}

/*
    Funzione cge effettua il calcolo del prodotto tra matrice e vettore
    - n = numero di righe della matrice
    - m = numero di colonne della matrice e numero di elementi del vettore
    - x = vettore di m elementi
    - A = matrice di n x m elementi
    - output: prodotto matrice-vettore
*/
int *matxvet(int n, int m, int *x, int **A){
    int i, //indice che socrrerà le righe della matrice
        j;  //indice che scorrerà le colonne della matrice e gli elementi del vettore
    int *ris; //variabile che conterrà il risultato del prodotto matrice-vettore
    struct timeval time; //variabile per il calcolo del tempo di esecuzione del prodotto
    double  startTime, //variabile contenente il tempo di inizio per il calcolo del prodotto 
            endTime; //variabile contenente il tempo di fine per il calcolo del prodotto

    //Allocazione memoria per il vettore risultato e inizializzazione degli elementi del vettore a 0
    ris = (int*) calloc (n, sizeof(int)); //il vettore ha tanti elementi quante sono le righe della matrice (n elementi)

    gettimeofday(&time, NULL);
    startTime=time.tv_sec+(time.tv_usec/1000000.0); //Salvataggio tempo iniziale

    /********** INIZIO REGIONE PARALLELA **********/
    /*  le variabili condivise tra i thread saranno:
        - A: matrice
        - x: vettore
        - m: numero colonne matrice ed elementi vettore
        - n: numero righe matrice
        - ris: variabile nella quale andrà salvato il risultato del prodotto

        le variabili locali ad ogni thread saranno i, j, indici utilizzati per scorrere
        la matrice ed il vettore
    */
    #pragma omp parallel for default(none) shared(A,x,m,n,ris) private(i,j) 
        for(i=0; i<n; i++) //iterazione su righe della matrice
            for(j=0; j<m; j++) //iterazione su colonne della matrice ed elementi dell'array
                //Calcolo parallelo del prodotto matrice-vettore
                ris[i] += A[i][j]*x[j]; //prodotto tra j-esimo delemento del vettore e j-esimo elemento della i-esima righa della matrice
    /********** FINE REGIONE PARALLELA **********/

    gettimeofday(&time, NULL); 
    endTime=time.tv_sec+(time.tv_usec/1000000.0); //Salvataggio tempo finale
    printf("\n Tempo impiegato: %f\n", endTime-startTime); //Stampa del tempo di esecuzione del prodotto matrice-vettore

    return ris; //Ritorno il vettore risultato
}

/*
    Funzione utilizzata per stampare i valori di un vettore
    - x = vettore da stampare
    - n = numero di elementi del vettore
    - arrayName = nome dell'array che verrà stampato a video
*/
void print_array(int *x, int n, char *arrayName){
    int i;

    printf("\n");
    for(i=0; i<n; i++)    
        printf(" %s[%d] = %d\n", arrayName, i, x[i]); 
    printf("\n");
}
