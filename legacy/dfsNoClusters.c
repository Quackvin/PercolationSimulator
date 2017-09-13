#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NCOLS 3
#define NROWS 3
#define P 0.6
#define PRECISION 5
#define DEBUG 1

typedef enum{false,true} bool;

typedef struct Node{
	bool north;
	bool south;
	bool east;
	bool west;
	bool seen;
}Node;

typedef struct Clusternode{
	struct Clusternode * prev;
	struct Clusternode * next;
	int row;
	int col;
}Clusternode;

typedef struct Cluster{
	Clusternode * head;
	Clusternode * tail;
	struct Cluster * prev;
	struct Cluster * next;
	int length;
}Cluster;

typedef struct Clusterlist{
	Cluster * head;
	Cluster * tail;
	int length;
}Clusterlist;

void randomiseLattice(Node lattice[][NCOLS]);
bool checkLattice(Node lattice[][NCOLS]);
void printLattice(Node lattice[][NCOLS],bool,bool);
void unseeLattice(Node lattice[][NCOLS]);
bool createBond();
void dfs(Node lattice[][NCOLS]);
void searchNode(Node lattice[][NCOLS], int, int, int);

int main(int argc, char* argv[]){
	// srand(time(NULL)); 

	Node lattice[NROWS][NCOLS];
	randomiseLattice(lattice);

	// check generation of bonds
	if(checkLattice(lattice) == 1) printf("valid lattice bonds\n");
	else printf("invalid lattice nbonds\n");

	// initialise lattice to all nodes unvisited and print a visualisation
	unseeLattice(lattice);
	printLattice(lattice,true,false);

	dfs(lattice);

	if(DEBUG){
		printLattice(lattice,false,true);
	}

	return 0;
}

void dfs(Node lattice[][NCOLS]){
	for(int i=0; i<NROWS; i++){
		for(int j=0; j<NCOLS; j++){
			if(lattice[i][j].seen == 0){
				searchNode(lattice,i,j,0);	
			}
		}
	}
}

void searchNode(Node lattice[][NCOLS], int row, int col, int depth){
	Node currNode = lattice[row][col];
	if(currNode.seen == 0){
		if(DEBUG){
			printf("\n\n___%d,%d, depth:[%d]\n", row,col,depth);
			printLattice(lattice,false,true);
		}
		// define neighbours with wrapping
		int prevRow = row - 1;
		if(prevRow < 0)
			prevRow = NROWS-1;
		int nextRow = row + 1;
		if(nextRow == NROWS)
			nextRow = 0;
		int prevCol = col - 1;
		if(prevCol < 0)
			prevCol = NCOLS-1;
		int nextCol = col + 1;
		if(nextCol == NCOLS)
			nextCol = 0;

		depth ++;
		
		if(DEBUG)
			printf("_____visited");

		// check Node and update cluster
		currNode.seen = true;

		// put checked Node into lattice
		lattice[row][col] = currNode;
		if(currNode.south == 1){
			if(DEBUG)
				printf(" trigger south\n");
			searchNode(lattice, nextRow, col, depth);
		}
		if(currNode.west == 1){
			if(DEBUG)
				printf(" trigger west\n");
			searchNode(lattice, row, prevCol, depth);
		}
		if(currNode.east == 1){
			if(DEBUG)
				printf(" trigger east\n");
			searchNode(lattice, row, nextCol, depth);
		}
		if(currNode.north == 1){
			if(DEBUG)
				printf(" trigger north\n");
			searchNode(lattice, prevRow, col, depth);
		}
	}
	else
		if(DEBUG)
			printf("_____skipped\n");
}

void randomiseLattice(Node lattice[][NCOLS]){
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

			bool hasBond = createBond();
			lattice[i][j].north = hasBond;
			lattice[previ][j].south = hasBond;
			hasBond = createBond();
			lattice[i][j].south = hasBond;
			lattice[nexti][j].north = hasBond;
			hasBond = createBond();
			lattice[i][j].west = hasBond;
			lattice[i][prevj].east = hasBond;
			hasBond = createBond();
			lattice[i][j].east = hasBond;
			lattice[i][nextj].west = hasBond;
		}
	}
}

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

			if(lattice[i][j].north != lattice[previ][j].south)
				return false;
			if(lattice[i][j].south != lattice[nexti][j].north)
				return false;
			if(lattice[i][j].west != lattice[i][prevj].east)
				return false;
			if(lattice[i][j].east != lattice[i][nextj].west)
				return false;
		}
	}
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

void unseeLattice(Node lattice[][NCOLS]){
	for(int i=0; i<NROWS; i++){
		for(int j=0; j<NCOLS; j++){
			lattice[i][j].seen = false;
		}
	}
}

bool createBond(){
	// determines precision used for site percolation probability
	int N = 1;
	for(int i=0; i<PRECISION; i++){
		N *= 10;
	}
	double randomn = (double)(rand()%N) / (double)N;
	if(DEBUG)
		printf("[%0.10f] ",randomn);
	if(randomn > P){
		return true;
	}
	return false;
}