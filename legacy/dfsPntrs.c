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
Clusterlist * dfs(Node lattice[][NCOLS]);
Cluster * searchNode(Node lattice[][NCOLS], int, int, int, Cluster *);
void testClusterlist(Clusterlist);

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

	Clusterlist * allClusters = dfs(lattice);

	if(DEBUG){
		printf("+++ %d %d\n",allClusters->head->next->head->row, allClusters->head->next->head->col);
		printf("-- %d %d\n", allClusters->length, allClusters->head->length);
		printLattice(lattice,false,true);
	}

	testClusterlist(*allClusters);

	return 0;
}

void testClusterlist(Clusterlist clist){
	printf("number of clusters: %d\n", clist.length);
	Cluster * c = clist.head;
	for(int i=0; i<clist.length; i++){
		Clusternode * n = c->head;
		printf("_cluster length: %d\n", c->length);
		for(int j=0; j<c->length; j++){
			printf("__row: %d col: %d ", n->row, n->col);
			n = n->next;
		}
		printf("\n");
		c = c->next;
	}
}

Clusterlist * dfs(Node lattice[][NCOLS]){
	Clusterlist * clusters = malloc(sizeof(Clusterlist));
	for(int i=0; i<NROWS; i++){
		for(int j=0; j<NCOLS; j++){
			if(lattice[i][j].seen == 0){
				/*		malloc cluster and use pointer logic		*/
				Cluster * cluster = malloc(sizeof(Cluster));
				cluster->length = 0;
				cluster = searchNode(lattice,i,j,0,cluster);
				if(DEBUG)
					printf("*****CLUSTER CREATED******\n");	

				// add cluster to cluster group linked list
				cluster->next = NULL;
				if(clusters->length == 0){
					cluster->prev = NULL;
					clusters->head = cluster;
				}
				else{
					cluster->prev = clusters->tail;
					cluster->prev->next = cluster;
				}
				clusters->tail = cluster;
				clusters->length++;

				if(!DEBUG){	
					printf("\n\ncluster length: %d\n", cluster->length);
					// printf("_head row: %d col %d\n", cluster->head->next->row,cluster->head->col);
				}
			}
		}
	}
	return clusters;
}

Cluster * searchNode(Node lattice[][NCOLS], int row, int col, int depth, Cluster * cluster){
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

		// add Node to cluster linked list
		Clusternode * node = malloc(sizeof(Clusternode));
		node->row = row;
		node->col = col;
		node->next = NULL;
		if(cluster->length == 0){
			node->prev = NULL;
			cluster->head = node;
		}
		else{
			node->prev->next = node;
			node->prev = cluster->tail;
		}
		cluster->tail = node;
		cluster->length++;

		if(!DEBUG)
			printf("\n***curr node:[%d], cluster tail:[%d], node prev:[%d]\n", node,cluster->tail,node->prev);


		// put checked Node into lattice
		lattice[row][col] = currNode;
		if(currNode.south == 1){
			if(DEBUG)
				printf(" trigger south\n");
			cluster = searchNode(lattice, nextRow, col, depth, cluster);
		}
		if(currNode.west == 1){
			if(DEBUG)
				printf(" trigger west\n");
			cluster= searchNode(lattice, row, prevCol, depth, cluster);
		}
		if(currNode.east == 1){
			if(DEBUG)
				printf(" trigger east\n");
			cluster = searchNode(lattice, row, nextCol, depth, cluster);
		}
		if(currNode.north == 1){
			if(DEBUG)
				printf(" trigger north\n");
			cluster = searchNode(lattice, prevRow, col, depth, cluster);
		}
	}
	else
		if(DEBUG)
			printf("_____skipped\n");
	return cluster;
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