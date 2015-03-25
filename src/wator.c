#include <stdio.h>
#include <stdlib.h> //per funzioni di random
#include <errno.h>
#include "wator.h"
#include <time.h>
#include <string.h>
#include <assert.h>

#define BUFFER_SIZE 100


int randVal(int sx, int dx){
	int range = dx - sx + 1; 
	int r;
	r = (rand() % range) + sx; //non proprio equo...
	return r;
}

/** trasforma una cella in un carattere
   \param a cella da trasformare

   \retval 'W' se a contiene WATER
   \retval 'S' se a contiene SHARK
   \retval 'F' se a contiene FISH
   \retval '?' in tutti gli altri casi
 */
char cell_to_char (cell_t a) {
	if(a==WATER)return 'W';
	if(a==SHARK)return 'S';
	if(a==FISH)return 'F';
	return '?';
}

/** trasforma un carattere in una cella
   \param c carattere da trasformare

   \retval WATER se c=='W'
   \retval SHARK se c=='S'
   \retval FISH se c=='F'
   \retval -1 in tutti gli altri casi
 */
int char_to_cell (char c) {
	if (c=='W') return WATER;
	if (c=='S') return SHARK;
	if (c=='F') return FISH;
	return -1;
}
/** crea un nuovo pianeta vuoto (tutte le celle contengono WATER) utilizzando 
    la rappresentazione con un vettore di puntatori a righe
    \param nrow numero righe
    \param numero colonne

    \retval NULL se si sono verificati problemi nell'allocazione
    \retval p puntatore alla matrice allocata altrimenti
*/
planet_t * new_planet (unsigned int nrow, unsigned int ncol){
	int i=0, j=0;
	planet_t* pianeta;
	pianeta=(planet_t*)malloc(sizeof(planet_t));
	if(pianeta==NULL){
		errno=ERANGE;
		perror("Pianeta non allocato");
		return NULL;
	}
	
	if((nrow<=0)||(ncol<=0)){
		errno=EINVAL;
		return NULL;
	}		
	/*Alloco spazio in memoria per le righe della matrice del pianeta*/
	cell_t **map;
	map=(cell_t**)malloc(nrow*sizeof(cell_t*));
	if(map==NULL) 
		return NULL; //ERRNO?
	
	/*Alloco spazio in memoria per le colonne della matrice del pianeta*/
	for(i=0;i<nrow;i++){
			map[i]=(cell_t*)malloc(ncol*sizeof(cell_t));
			if(map[i]==NULL)
				return NULL; //ERRNO?
	}
	/*Setto la matrice a WATER*/
	for(i=0;i<nrow;i++)
		for(j=0;j<ncol;j++)
			map[i][j]=WATER;
	
	pianeta->w=map;
	pianeta->nrow=nrow;
	pianeta->ncol=ncol;
	return pianeta;
}

/** dealloca un pianeta (e tutta la matrice ...)
    \param p pianeta da deallocare

*/

void free_planet (planet_t* p){
	int i;
	/*FREE MATRICE*/
	if(p->ncol>0){
		for(i=0;i<p->nrow;i++)
			free((p->w[i]));
		/*FREE BTIME*/

		/*FREE PTIME*/

		free(p);
	}
}

/** stampa il pianeta su file secondo il formato di fig 2 delle specifiche, es

3
5
W F S W W
F S W W S
W W W W W

dove 3 e' il numero di righe (seguito da newline \n)
5 e' il numero di colonne (seguito da newline \n)
e i caratteri W/F/S indicano il contenuto (WATER/FISH/SHARK) separati da un carattere blank (' '). Ogni riga terminata da newline \n

    \param f file su cui stampare il pianeta (viene sovrascritto se esiste)
    \param p puntatore al pianeta da stampare

    \retval 0 se tutto e' andato bene
    \retval -1 se si e' verificato un errore (in questo caso errno e' settata opportunamente)

*/

int print_planet (FILE* f, planet_t* p){
	int i,j;
	/*Print righe e colonne*/
	//fprintf(stdout, "TRY: PRINT\n");
	if(p==NULL)
		return -1;
	//fprintf(stdout, "VOID CONDITION JUMPED\n");
	if((p->nrow<=0)||(p->ncol<=0)){/*SET ERRNO*/return -1;}
	//fprintf(stdout, "CHECK DONE\nPRINTING:\n");
	fprintf(f, "%d\n",p->nrow);
	fprintf(f, "%d\n",p->ncol);
	/*Print matrice
	scorro ogni cella, converto il suo valore in CHAR e lo stampo*/
	for(i=0;i<p->nrow;i++){
		for(j=0;j<p->ncol;j++){
			char a=cell_to_char(p->w[i][j]);
			if(a=='?'){/*SET ERRNO*/return -1;}
			fprintf(f, "%c",a);
			if(j<(p->ncol)-1)
				fprintf(f, " ");
		}
		fprintf(f, "\n");
	}
			//fprintf(f, "PRINTED\n");
	return 0;
}


/** inizializza il pianeta leggendo da file la configurazione iniziale

    \param f file da dove caricare il pianeta (deve essere gia' stato aperto in lettura)

    \retval p puntatore al nuovo pianeta (allocato dentro la funzione)
    \retval NULL se si e' verificato un errore (setta errno) 
            errno = ERANGE se il file e' mal formattato
 */

planet_t* load_planet (FILE* f){ 
	int i,j;
	char nrow_c[BUFFER_SIZE], ncol_c[BUFFER_SIZE];
	planet_t* pi;

	/*Prendo il numero di righe e controllo se ci sono errori
	Converto il numero di righe preso da una stringa CHAR* in un INT
	Posso usare la funzione atoi() perché l'input è <= BUFFER_SIZE*/
	
	if(fgets(nrow_c, BUFFER_SIZE, f)==NULL){
		/*SET ERRNO*/
		return NULL;
	}
	i=atoi(nrow_c);

	/*Prendo il numero di colonne e controllo se ci sono errori*/
	if(fgets(ncol_c, BUFFER_SIZE, f)==NULL){
		/*SET ERRNO*/ 
		return NULL;
	}
	j=atoi(ncol_c);
	
	/*Se il numero di righe o colonne è <=0 allora ho un errore
	altrimenti provo ad allocare il pianeta
	controllo se l'allocazione è andata a buon fine
	se no restituisco errore, altrimenti copio i parametri*/
	if((i<=0)||(j<=0)){/*SET ERRNO*/ return NULL;}
	/*Alloco lo spazio per il pianeta*/
	pi=new_planet(i, j);
	if (pi==NULL){
		/*SET ERRNO*/ 
		return NULL;
	}
	pi->nrow=i;
	pi->ncol=j;
	
	/*Prendo in input la matrice (carattere per carattere)
	converto ogni carattere preso da CHAR a cell_t
	se non ci sono errori lo salvo nella giusta posizione sulla matrice*/
	for(i=0;i<pi->nrow;i++)
		for(j=0;j<pi->ncol;j++){
			char var;
			if (fgets(&var, 2, f)==NULL){
				/*SET ERRNO*/
				return NULL;}
			pi->w[i][j]=char_to_cell(var);
			if(pi->w[i][j]==-1){
				/*SET ERRNO*/
				return NULL;}
			/*Questo carattere dovrebbe essere vuoto
			se non lo è, allora il file è formattato male*/
			fgets(&var, 2, f);
			if((var!=' ')&&(var!='\n')){
				/*SET ERRNO*/
				return NULL;}
		}
		return(pi);
	//assert(pi==NULL);
}


/** crea una nuova istanza della simulazione in base ai contenuti 
    del file di configurazione "wator.conf"

    \param fileplan nome del file da cui caricare il pianeta

    \retval p puntatore alla nuova struttura che descrive 
              i parametri di simulazione
    \retval NULL se si e' verificato un errore (setta errno)
*/
wator_t* new_wator (char* fileplan){
	srand(time(NULL));
	FILE* f;
	/*Apro il file e controllo se ci sono stati errori nell'apertura*/
	f=fopen(fileplan, "r");
	if(f==NULL){
		/*SET ERRNO*/
		return NULL;}
	/*Creo il pianeta e controllo se ci sono stati errori*/
	planet_t* plan;
	plan=load_planet(f);
	if(plan==NULL){
		/*SET ERRNO*/
		return NULL;}
	
	/*Creo la simulazione e controllo se ci sono stati errori
	se no, assegno il pianeta creato prima alla nuova simulazione*/
	wator_t* acquarium;
	acquarium=(wator_t*)malloc(sizeof(wator_t));
	if(acquarium==NULL){
		/*SET ERRNO*/
		return NULL;}
	acquarium->plan=plan;
	acquarium->sd=0;
	acquarium->sb=0;
	acquarium->fb=0;
	acquarium->nf=0;
	acquarium->ns=0;
	acquarium->nwork=0;
	acquarium->chronon=0;
	
	int var;
	int sd=0, sb=0, fb=0;
	char* str;
	char* in;
	str=(char*)malloc(sizeof(char)*3);
	in=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	
	/*Leggo il primo parametro e setto il contatore
	(il contatore mi serve per vedere se in futuro leggerò duplicati)
	assegno il valore giusto nella simulazione
	*/
	#define NCHAR 3 /*leggo 2 caratteri*/
	FILE* conf;
	conf = fopen("wator.conf","r");
	if(conf==NULL){
		/*SET ERRNO*/
		return NULL;}
	if(fgets(str, NCHAR, conf)==NULL){
		/*SET ERRNO*/
		return NULL;}

	if(fgets(in, BUFFER_SIZE, conf)==NULL){
		/*SET ERRNO*/
		return NULL;}
	var=atoi(in);
	
	if(!strncmp(str,"sd",3)){
		acquarium->sd=var;
		sd=1;
	}
	else if(!strncmp(str,"sb",3)){
		acquarium->sb=var;
		sb=1;
	}
	else if(!strncmp(str,"fb",3)){
		acquarium->fb=var;
		fb=1;
	}
	else {
		/*SET ERRNO*/
		return NULL;}
	
	/*Leggo gli altri valori, guardando se sono formattati correttamente 
	e se ho duplicati*/
	while(fgets(str, NCHAR, conf)){
		fgets(in, BUFFER_SIZE, conf);
		if(in==NULL){
			/*SET ERRNO*/
			return NULL;}
		var=atoi(in);
	
		if(!strncmp(str,"sd",3)){
			if(sd==0)
				acquarium->sd=var;
			//else ho un duplicato.
			sd=1;
		}
		else if(!strncmp(str,"sb",3)){
			if(sb==0)
				acquarium->sb=var;
			//else errore?
			sb=1;
		}
		else if(!strncmp(str,"fb",3)){
			if(fb==0)
				acquarium->fb=var;
			//else errore?
			fb=1;
		}
		else {
			/*SET ERRNO*/
			return NULL;}
	}
	if((!sd)||(!sb)||(!fb))
		return NULL;
	
	return acquarium;
}

/** libera la memoria della struttura wator (e di tutte le sottostrutture)
    \param pw puntatore struttura da deallocare

 */
void free_wator(wator_t* pw){
	free_planet(pw->plan);
	free(pw);
}

#define STOP 0
#define EAT  1
#define MOVE 2
#define ALIVE 3
#define DEAD 4
/** Regola 1: gli squali mangiano e si spostano 
  \param pw puntatore alla struttura di simulazione
  \param (x,y) coordinate iniziali dello squalo
  \param (*k,*l) coordinate finali dello squalo (modificate in uscita)

  La funzione controlla i 4 vicini 
              (x-1,y)
        (x,y-1) *** (x,y+1)
              (x+1,y)

  Se una di queste celle contiene un pesce, lo squalo mangia il pesce e 
  si sposta nella cella precedentemente occupata dal pesce. Se nessuna 
  delle celle adiacenti contiene un pesce, lo squalo si sposta
  in una delle celle adiacenti vuote. Se ci sono piu' celle vuote o piu' pesci 
  la scelta e' casuale.
  Se tutte le celle adiacenti sono occupate da squali 
  non possiamo ne mangiare ne spostarci lo squalo rimane fermo.

  NOTA: la situazione del pianeta viene 
        modificata dalla funzione con il nuovo stato

  \retval STOP se siamo rimasti fermi
  \retval EAT se lo squalo ha mangiato il pesce
  \retval MOVE se lo squalo si e' spostato solamente
  \retval -1 se si e' verificato un errore (setta errno)
*/

int shark_rule1 (wator_t* pw, int x, int y, int *k, int* l){
	cell_t lt, rt, up, dn;	
	
	//CONTROLLO ERRORI
	if (pw==NULL){
		/*SET ERRNO*/
		return -1;}
	if (pw->plan==NULL){
		/*SET ERRNO*/
		return -1;}
	if (pw->plan->w==NULL){
		/*SET ERRNO*/
		return -1;}
	if((x<0)||(x>pw->plan->nrow)||(y<0)||(y>pw->plan->ncol)){
		/*SET ERRNO*/
		return -1;}
	if(pw->plan->w[x][y]!=SHARK){
		/*SET ERRNO*/
		return -1;}
	
	/*copio i valori adiacenti alla cella (se presenti)*/
	if (x>0) up = pw->plan->w[x-1][y];
	else up = pw->plan->w[pw->plan->nrow][y];
	
	if (x<pw->plan->nrow) dn = pw->plan->w[x+1][y];
	else dn = pw->plan->w[0][y];
	
	if (y>0) lt = pw->plan->w[x][y-1];
	else lt = pw->plan->w[x][pw->plan->ncol];
	
	if (x<pw->plan->ncol) rt = pw->plan->w[x][y+1];
	else rt = pw->plan->w[x][0];
	
	/*se le celle adiacenti (se presenti) contengono SHARK allora STOP*/
	if((up==SHARK)&&(dn==SHARK)&&(lt==SHARK)&&(rt==SHARK)){
		*k=x;
		*l=y;
		return STOP;
	}
	
	/*conto FISH adiacenti*/
	int nFISH=0;
	if(up==FISH)nFISH++;
	if(rt==FISH)nFISH++;
	if(dn==FISH)nFISH++;
	if(lt==FISH)nFISH++;
	
	/*scelgo FISH a caso (se presenti) e sposto SHARK, restituisco EAT*/
	if(nFISH>0){
		int randF;
		randF = randVal(1, nFISH);
		if(randF>1){if(up==FISH) randF--;}
		else{ /*SHARK mangia FISH in alto*/
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x-1][y]=SHARK;
			*k=x-1;
			*l=y;
			return EAT;}
		if(randF>1){if(rt==FISH) randF--;}
		else{ /*SHARK mangia FISH a destra*/
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x][y+1]=SHARK;
			*k=x;
			*l=y+1;
			return EAT;}
		if(randF>1){if(dn==FISH) randF--;}
		else{ //SHARK mangia FISH in basso
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x+1][y]=SHARK;
			*k=x+1;
			*l=y;
			return EAT;}
		//SHARK mangia FISH a sinistra
		pw->plan->w[x][y]=WATER;
		pw->plan->w[x][y-1]=SHARK;
		*k=x;
		*l=y-1;
		return EAT;
	}
	
	//se non ci sono FISH, conto WATER adiacenti
	int nWATER=0;
	if(up==WATER)nWATER++;
	if(rt==WATER)nWATER++;
	if(dn==WATER)nWATER++;
	if(lt==WATER)nWATER++;
	
	//scelgo cella WATER a caso e sposto SHARK, restituisco MOVE
	if(nWATER>0){
		int randW;
		randW = randVal(1, nWATER);
		if(randW>1){if(up==WATER) randW--;}
		else{ //SHARK si sposta in alto
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x-1][y]=SHARK;
			*k=x-1;
			*l=y;
			return MOVE;}
		if(randW>1){if(rt==WATER) randW--;}
		else{ //SHARK si sposta a destra
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x][y+1]=SHARK;
			*k=x;
			*l=y+1;
			return MOVE;}
		if(randW>1){if(dn==WATER) randW--;}
		else{ //SHARK si sposta in basso
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x+1][y]=SHARK;
			*k=x+1;
			*l=y;
			return MOVE;}
		//SHARK si sposta a sinistra
		pw->plan->w[x][y]=WATER;
		pw->plan->w[x][y-1]=SHARK;
		*k=x;
		*l=y-1;
		return MOVE;
	}
	
}

/** Regola 2: gli squali si riproducono e muoiono
  \param pw puntatore alla struttura wator
  \param (x,y) coordinate dello squalo
  \param (*k,*l) coordinate dell'eventuale squalo figlio (

  La funzione calcola nascite e morti in base agli indicatori 
  btime(x,y) e dtime(x,y).

  == btime : nascite ===
  Se btime(x,y) e' minore di  pw->sb viene incrementato.
  Se btime(x,y) e' uguale a pw->sb si tenta di generare un nuovo squalo.
  Si considerano i 4 vicini 
              (x-1,y)
        (x,y-1) *** (x,y+1)
              (x+1,y)

  Se una di queste celle e' vuota lo squalo figlio viene generato e la occupa, se le celle sono tutte occupate da pesci o squali la generazione non avviene. 
  In entrambi i casi btime(x,y) viene azzerato.

  == dtime : morte dello squalo  ===
  Se dtime(x,y) e' minore di pw->sd viene incrementato.
  Se dtime(x,y) e' uguale a pw->sd lo squalo muore e la sua posizione viene 
  occupata da acqua.

  NOTA: la situazione del pianeta viene 
        modificata dalla funzione con il nuovo stato


  \retval DIED se lo squalo e' morto
  \retval ALIVE se lo squalo e' vivo
  \retval -1 se si e' verificato un errore (setta errno)
*/
int shark_rule2 (wator_t* pw, int x, int y, int *k, int* l);

/** Regola 3: i pesci si spostano

    \param pw puntatore alla struttura di simulazione
    \param (x,y) coordinate iniziali del pesce
    \param (*k,*l) coordinate finali del pesce

    La funzione controlla i 4 vicini 
              (x-1,y)
        (x,y-1) *** (x,y+1)
              (x+1,y)

     un pesce si sposta casualmente in una delle celle adiacenti (libera). 
     Se ci sono piu' celle vuote la scelta e' casuale. 
     Se tutte le celle adiacenti sono occupate rimaniamo fermi.

     NOTA: la situazione del pianeta viene 
        modificata dalla funzione con il nuovo stato

  \retval STOP se siamo rimasti fermi
  \retval MOVE se il pesce si e' spostato
  \retval -1 se si e' verificato un errore (setta errno)
*/
int fish_rule3 (wator_t* pw, int x, int y, int *k, int* l){
	cell_t lt, rt, up, dn;	
	
	//CONTROLLO ERRORI
	if (pw==NULL){
		/*SET ERRNO*/
		return -1;}
	if (pw->plan==NULL){
		/*SET ERRNO*/
		return -1;}
	if (pw->plan->w==NULL){
		/*SET ERRNO*/
		return -1;}
	if((x<0)||(x>pw->plan->nrow)||(y<0)||(y>pw->plan->ncol)){
		/*SET ERRNO*/
		return -1;}
	if(pw->plan->w[x][y]!=FISH){
		/*SET ERRNO*/
		return -1;}
	
	//copio i valori adiacenti alla cella (se presenti)
	if (x>0) up = pw->plan->w[x-1][y];
	else up = -1;
	
	if (x<pw->plan->nrow) dn = pw->plan->w[x+1][y];
	else dn = -1;
	
	if (y>0) lt = pw->plan->w[x][y-1];
	else lt = -1;
	
	if (x<pw->plan->ncol) rt = pw->plan->w[x][y+1];
	else rt = -1;
	
	//se le celle adiacenti (se presenti) non sono WATER allora STOP
	if(((up!=WATER)||(up==-1))&&((dn!=WATER)||(dn==-1))&&((lt!=WATER)||(lt==-1))&&((rt!=WATER)||(rt==-1))){
		*k=x;
		*l=y;
		return STOP;
	}
	
	//conto le celle WATER adiacenti
	int nWATER=0;
	if(up==WATER)nWATER++;
	if(rt==WATER)nWATER++;
	if(dn==WATER)nWATER++;
	if(lt==WATER)nWATER++;
	
	//scelgo cella WATER a caso e sposto FISH, restituisco MOVE
	if(nWATER>0){
		int randW;
		randW = randVal(1, nWATER);
		if(randW>1){
			if(up==WATER)
				randW--;}
		else{ //FISH si sposta in alto
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x-1][y]=FISH;
			*k=x-1;
			*l=y;
			return MOVE;}
		if(randW>1){
			if(rt==WATER)
				randW--;}
		else{ //FISH si sposta a destra
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x][y+1]=FISH;
			*k=x;
			*l=y+1;
			return MOVE;}
		if(randW>1){
			if(dn==WATER)
				randW--;}
		else{ //FISH si sposta in basso
			pw->plan->w[x][y]=WATER;
			pw->plan->w[x+1][y]=FISH;
			*k=x+1;
			*l=y;
			return MOVE;}
		//FISH si sposta a sinistra
		pw->plan->w[x][y]=WATER;
		pw->plan->w[x][y-1]=FISH;
		*k=x;
		*l=y-1;
		return MOVE;
	}
}

/** Regola 4: i pesci si riproducono
  \param pw puntatore alla struttura wator
  \param (x,y) coordinate del pesce
  \param (*k,*l) coordinate dell'eventuale pesce figlio 

  La funzione calcola nascite in base a btime(x,y) 

  Se btime(x,y) e' minore di  pw->sb viene incrementato.
  Se btime(x,y) e' uguale a pw->sb si tenta di generare un nuovo pesce.
  Si considerano i 4 vicini 
              (x-1,y)
        (x,y-1) *** (x,y+1)
              (x+1,y)

  Se una di queste celle e' vuota il pesce figlio viene generato e la occupa, se le celle sono tutte occupate da pesci o squali la generazione non avviene. 
  In entrambi i casi btime(x,y) viene azzerato.

  NOTA: la situazione del pianeta viene 
        modificata dalla funzione con il nuovo stato


  \retval  0 se tutto e' andato bene
  \retval -1 se si e' verificato un errore (setta errno)
*/
int fish_rule4 (wator_t* pw, int x, int y, int *k, int* l);


/** restituisce il numero di pesci nel pianeta
    \param p puntatore al pianeta
    
    \retval n (>=0) numero di pesci presenti
    \retval -1 se si e' verificato un errore (setta errno )
 */
int fish_count (planet_t* p){
	int i,j;
	if(p==NULL){
		/*SET ERRNO*/
			return -1;}
	if(p->w==NULL){
		/*SET ERRNO*/
		return -1;}
	int nFISH=0;
	for(i=0;i<p->nrow; i++)
		for(j=0;j<p->ncol; j++)
			if(p->w[i][j]==FISH)
				nFISH++;
	return nFISH;
}

/** restituisce il numero di squali nel pianeta
    \param p puntatore al pianeta
    
    \retval n (>=0) numero di squali presenti
    \retval -1 se si e' verificato un errore (setta errno )
 */
int shark_count (planet_t* p){
	int i,j;
	if(p==NULL){
		/*SET ERRNO*/
		return -1;}
	if(p->w==NULL){
		/*SET ERRNO*/
		return -1;}
	int nSHARK=0;
	for(i=0;i<p->nrow; i++)
		for(j=0;j<p->ncol; j++)
			if(p->w[i][j]==SHARK)
				nSHARK++;
	return nSHARK;
}


/** calcola un chronon aggiornando tutti i valori della simulazione e il pianeta
   \param pw puntatore al pianeta

   \return 0 se tutto e' andato bene
   \return -1 se si e' verificato un errore (setta errno)

 */
int update_wator (wator_t * pw){
	

}
