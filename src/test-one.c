/**
   \file
   \author lso15 
   
   \brief Test primo frammento
 */

#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>
#include <errno.h>

#include "wator.h"
#include "wator.c"

/** file test load/store */
#define FILETEST1 "planet1.check"
#define FILETEST2 "planet2.check"
#define FILETEST3 "planet3.check"
#define FILEOUT1 "out1.check"
#define FILEOUT2 "out2.check"


/** main di test */
int main (void) {

  planet_t * p;
  wator_t * pw;
  FILE* fsa, *fsb;

  mtrace();

  /* 
     test funzioni di trasformazione caratteri celle e viceversa
  */
  
  if ( cell_to_char (WATER) != 'W' || cell_to_char(210) != '?' )  {
    fprintf(stderr,"Test cell 1 \n");
    exit(EXIT_FAILURE);
    }

  if ( char_to_cell ('F') != FISH || char_to_cell ('?') != -1 )  {
    fprintf(stderr,"Test cell 2 \n");
    exit(EXIT_FAILURE);
    }

  if ( cell_to_char(char_to_cell ('S')) != 'S' ||  char_to_cell ( cell_to_char( SHARK ) ) != SHARK )  {
    fprintf(stderr,"Test cell 3 \n");
    exit(EXIT_FAILURE);
    }

  
  /* test creazione e stampa nuovo pianeta*/
  /* argomenti errati */
  if ( new_planet(0,30) != NULL && errno != EINVAL ) {
    fprintf(stderr,"Test planet 1\n");
    exit(EXIT_FAILURE);
    }

  if ( new_planet(30000,0) != NULL && errno != EINVAL ) {
    fprintf(stderr,"Test planet 2\n");
    exit(EXIT_FAILURE);
    }

  /* pianeta 10x10 */
  if (  ( p = new_planet(10,10) ) == NULL ) {
    fprintf(stderr,"Test planet 3\n");
    exit(EXIT_FAILURE);
    }

  print_planet(stdout,p);
		//fprintf(stdout, "PRINT PLANET DONE\n");
  free_planet(p);
  //fprintf(stdout, "FREE PLANET DONE\n");

  /* pianeta 10000x10000 */
  if (  ( p = new_planet(10000,10000) ) == NULL ) {
    fprintf(stderr,"Test planet 4\n");
    exit(EXIT_FAILURE);
    }

  free_planet(p);

 
  /* test 1 su load/store pianeta da file */
  //fprintf(stdout,"TRY: OPEN FILEOUT1\n");//TEST
  fsa=fopen(FILEOUT1,"r");

  if ( fsa == NULL ) {
    perror("fopen1");
    exit(EXIT_FAILURE);
  }

	//fprintf(stdout,"TRY: OPEN FILETEST1\n");
  fsb=fopen(FILETEST1,"w");
  if ( fsb == NULL ) {
    perror("fopen2");
    exit(EXIT_FAILURE);
  }
	//fprintf(stdout,"TRY: LOAD PLANET\n");
  p=load_planet(fsa);
  //fprintf(stdout,"TRY: PRINT PLANET (FSB)\n");
  print_planet(fsb,p);
  //fprintf(stdout,"TRY: PRINT PLANET (STDOUT)\n");
  print_planet(stdout,p);
  free_planet(p);
  fclose(fsa); 
  fclose(fsb);

  
  /* test 2 su load/store pianeta da file */
  
  fsa=fopen(FILEOUT2,"r");
  if ( fsa == NULL ) {
    perror("fopen3");
    exit(EXIT_FAILURE);
  }

  fsb=fopen(FILETEST2,"w");
  if ( fsb == NULL ) {
    perror("fopen4");
    exit(EXIT_FAILURE);
  }
  p=load_planet(fsa);
  print_planet(fsb,p);
  free_planet(p);
  fclose(fsa);
  fclose(fsb);

  /* test creazione struttura wator (wator.conf.1) */
    
  if ( ( pw = new_wator(FILETEST1) ) == NULL ) {
    fprintf(stderr,"Test wator 1\n");
    exit(EXIT_FAILURE);
    }  

  if ( (pw->sd != 100) || (pw->sb != 20) || (pw->fb != 15) )  {
    fprintf(stderr,"Test wator 2\n");
    exit(EXIT_FAILURE);
    }  


  fsb=fopen(FILETEST3,"w");
  if ( fsb == NULL ) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  print_planet(fsb,pw->plan);

  fclose(fsb);

  free_wator(pw);

  muntrace();

  return EXIT_SUCCESS;
}
