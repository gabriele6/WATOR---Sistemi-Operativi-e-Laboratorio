/**
   \file
   \author lso15 

   \brief struttura ad albero relativa agli utenti connessiwator
*/ 
#define BUFFER_SIZE 100
#ifndef __WATOR__H
#define __WATOR__H

#include <stdio.h>

/** file di configurazione */
static const char CONFIGURATION_FILE[] = "wator.conf";

/** tipo delle celle del pianeta:
   SHARK squalo
   FISH pesce
   WATER solo acqua

*/
typedef enum cell { SHARK, FISH, WATER } cell_t;


/** Tipo matrice acquatica che rappreseneta il pianeta */
typedef struct planet {
  /** righe */
  unsigned int nrow; 
  /** colonne */
  unsigned int ncol; 
  /** matrice pianeta */
  cell_t ** w;
  /** matrice contatori nascita (pesci e squali)*/
  int ** btime;
  /** matrice contatori morte (squali )*/
  int ** dtime;

} planet_t;

/** struttura che raccoglie le informazioni di simulazione */
typedef struct wator {
  /** sd numero chronon morte squali per digiuno */
  int sd; 
  /** sb numero chronon riproduzione squali */
  int sb; 
  /** fb numero chronon riproduzione pesci */
  int fb; 
  /** nf numero pesci*/
  int nf; 
  /** ns numero squali */
  int ns; 
  /** numero worker */
  int nwork;
  /** durata simulazione */
  int chronon;
  /** pianeta acquatico */
  planet_t* plan;
} wator_t;

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
	
	if((nrow<=0)||(ncol<=0))return NULL;	
	
	cell_t **map;
	map=(cell_t**)malloc(nrow*sizeof(cell_t*));
	if(map==NULL)return NULL; 						//errno?
	
	for(i=0;i<nrow;i++){
		*map[i]=(cell_t*)malloc(sizeof(cell_t)*ncol);
		if(*map[i]==NULL)return NULL;
	}		
	for (i=0;i<nrow;i++)
		for(j=0;j<ncol;j++)
			map[i][j]=WATER;
	
	*pianeta.w=map;
	*pianeta.nrow=nrow;
	*pianeta.ncol=ncol;
	return pianeta;
}

/** dealloca un pianeta (e tutta la matrice ...)
    \param p pianeta da deallocare

*/
void free_planet (planet_t* p){
	int i,j;
	for(i=0;i<*p.nrow;i++)
		for(j=0;j<*p.ncol;j++)
			free(*p.w[i][j]);
	free(*p);
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

	fprintf(f, "%d\n",*p.nrow);
	fprintf(f, "%d\n",*p.ncol);
	for(i=0;i<*p.nrow;i++){
		for(j=0;j<*p.ncol;j++){
			fprintf(f, "%c",cell_to_char(*p.w[i][j]));
			if(j<*p.ncol-1)
				fprintf(f, " ");
		}
		fprintf(f, "\n");
	}
}


/** inizializza il pianeta leggendo da file la configurazione iniziale

    \param f file da dove caricare il pianeta (deve essere gia' stato aperto in lettura)

    \retval p puntatore al nuovo pianeta (allocato dentro la funzione)
    \retval NULL se si e' verificato un errore (setta errno) 
            errno = ERANGE se il file e' mal formattato
 */
planet_t* load_planet (FILE* f){ //SOLO PARAMETRI O MATRICE INTERA???
	int i,j;
	char nrow_c[BUFFER_SIZE], ncol_c[BUFFER_SIZE];
	planet_t* planeta;
	planeta=(planet_t*)malloc(sizeof(planet_t));
	if (planeta==NULL) return NULL;
	//SETTARE ERRNO
	fgets(nrow_c, BUFFER_SIZE, f);
	if(nrow_c==NULL) return NULL;
	i=atoi(nrow_c);

	fgets(ncol_c, BUFFER_SIZE, f);
	if(ncol_c==NULL) return NULL;
	j=atoi(nrow_c);
	
	if((i<=0)||(j<=0)){/*SET ERRNO*/ return NULL;}
	*pianeta.nrow=i;
	*pianeta.nrow=j;
		
	//SE MATRICE INTERA:
	for(i=0;i<*planeta.nrow;i++)
		for(j=0;j<*planeta.ncol;j++){
			char var;
			fgets(&var, 1, f);
			if (var==NULL) /*SET ERRNO*/ return NULL;
			*planeta.w[i][j]=char_to_cell(var);
			if(*planeta.w[i][j]==-1){/*SET ERRNO*/ return NULL;}
		}
}


/** crea una nuova istanza della simulazione in base ai contenuti 
    del file di configurazione "wator.conf"

    \param fileplan nome del file da cui caricare il pianeta

    \retval p puntatore alla nuova struttura che descrive 
              i parametri di simulazione
    \retval NULL se si e' verificato un errore (setta errno)
*/
wator_t* new_wator (char* fileplan){
	FILE* f;
	f=open(fileplan, "r");
	if(f==-1){/*SET ERRNO*/ return NULL;}
	planet_t* plan;
	plan=load_planet(f);
	if(plan==NULL){/*SET ERRNO*/ return NULL;}
	
	wator_t* acquarium;
	acquarium=(wator_t*)malloc(sizeof(wator_t));
	if(acquarium==NULL){/*SET ERRNO*/ return NULL;}
	*acquarium.plan=plan;
	*acquarium.sd=0;
	*acquarium.sb=0;
	*acquarium.fb=0;
	*acquarium.nf=0;
	*acquarium.ns=0;
	*acquarium.nwork=0;
	*acquarium.chronon=0;
	
	int var;
	int sd=0, sb=0; fb=0;
	char str[3], in[BUFFER_SIZE];
	
	#define NCHAR 3
	fgets(str, NCHAR, f); //QUANTI CARATTERI LEGGO? 2 O 3?
	if(str==NULL){/*SET ERRNO*/return NULL;}
	fgets(in, BUFFER_SIZE, f); //COSÌ LEGGO ANCHE LO SPAZIO?
	if(in==NULL){/*SET ERRNO*/ return NULL;}
	var=atoi(in);
	
	if(str=="sd"){
		*acquarium.sd=var;
		sd=1;
	}
	else if(str=="sb"){
		*acquarium.sb=var;
		sb=1;
	}
	else if(str=="fb"){
		*acquarium.fb=var;
		fb=1;
	}
	else {/*SET ERRNO*/ return NULL;}
	
	while(fgets(str, NCHAR, f)){
		fgets(in, BUFFER_SIZE, f);
		if(in==NULL){/*SET ERRNO*/ return NULL;}
		var=atoi(in);
	
		if(str=="sd"){
			if(sd==0)
				*acquarium.sd=var;
			//else errore?
			sd=1;
		}
		else if(str=="sb"){
			if(sb==0)
				*acquarium.sb=var;
			//else errore?
			sb=1;
		}
		else if(str=="fb"){
			if(fb==0)
				*acquarium.fb=var;
			//else errore?
			fb=1;
		}
		else {/*SET ERRNO*/ return NULL;}
	}
	return acquarium;
}

/** libera la memoria della struttura wator (e di tutte le sottostrutture)
    \param pw puntatore struttura da deallocare

 */
void free_wator(wator_t* pw){
	free_planet(*pw.plan);
	free(*pw);
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
	int* k,l;
	if((*pw.*plan.w[x-1][y])==(*pw.*plan.w[x+1][y])==(*pw.*plan.w[x][y-1])==(*pw.*plan.w[x][y+1])==SHARK){*k=x; *l=y; return STOP;}
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
int fish_rule3 (wator_t* pw, int x, int y, int *k, int* l);

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
int fish_count (planet_t* p);

/** restituisce il numero di squali nel pianeta
    \param p puntatore al pianeta
    
    \retval n (>=0) numero di squali presenti
    \retval -1 se si e' verificato un errore (setta errno )
 */
int shark_count (planet_t* p);


/** calcola un chronon aggiornando tutti i valori della simulazione e il pianeta
   \param pw puntatore al pianeta

   \return 0 se tutto e' andato bene
   \return -1 se si e' verificato un errore (setta errno)

 */
int update_wator (wator_t * pw);

#endif
