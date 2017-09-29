#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "nodes.h"

#define DEBUG 0

bool checkLattice(Node lattice[][NCOLS]){
	for(int i=0; i<NROWS; i++){
		int previ = i-1;
		if(previ < 0) 
			previ = NROWS-1;
		int nexti = i+1;
		if(nexti == NROWS)
			nexti = 0;

		for(int j=0; j<NCOLS; j++){
			int prevj = j-1;
			if(prevj < 0)
				prevj = NCOLS-1;
			int nextj = j+1;
			if(nextj == NCOLS)
				nextj = 0;

			if(lattice[i][j].north != lattice[previ][j].south){
				printf("invalid lattice\n");
				return false;
			}
			if(lattice[i][j].south != lattice[nexti][j].north){
				printf("invalid lattice\n");
				return false;
			}
			if(lattice[i][j].west != lattice[i][prevj].east){
				printf("invalid lattice\n");
				return false;
			}
			if(lattice[i][j].east != lattice[i][nextj].west){
				printf("invalid lattice\n");
				return false;
			}
		}
	}
	printf("valid lattice\n");
	return true;
}

void printLattice(Node lattice[][NCOLS], bool cons, bool seen){
	if(cons){
		printf("connections\n");
		for(int i=0; i<NROWS; i++){
			for(int j=0; j<NCOLS; j++){
				printf("[%d %d %d %d] ",lattice[i][j].west,lattice[i][j].north,lattice[i][j].south,lattice[i][j].east);
			}
			printf("\n");
		}
	}
	if(seen){
		printf("seen\n");
		for(int i=0; i<NROWS; i++){
			for(int j=0; j<NCOLS; j++){
				printf("[%d] ",lattice[i][j].seen);
			}
			printf("\n");
		}
	}
	
}