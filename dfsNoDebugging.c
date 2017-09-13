#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NCOLS 1024
#define NROWS 1024
#define P 0.6
#define PRECISION 5
#define DEBUG 0

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
void printLattice(Node lattice[][NCOLS],bool showConnections,bool showSeen);
void unseeLattice(Node lattice[][NCOLS]);
bool createBond();

Clusterlist * dfs(Node lattice[][NCOLS]);
void searchNode(Node lattice[][NCOLS], int row, int col, int depth, Cluster *);

Clusterlist * newClusterlist();
void addClusterToClusterlist(Clusterlist *, Cluster *);
Cluster * newCluster();
void addNodeToCluster(Cluster *, Clusternode *);
Clusternode * newClusternode(int row, int col);
void clustersTest(Clusterlist * clusterlist);


int main(int argc, char* argv[]){
	srand(time(NULL)); 

	Node lattice[NROWS][NCOLS];
	randomiseLattice(lattice);

	// initialise lattice to all nodes unvisited and print a visualisation
	unseeLattice(lattice);

	// printLattice(lattice,true,false);

	Clusterlist * allClusters = dfs(lattice);

	return 0;
}

void clustersTest(Clusterlist * clusterlist){
	Cluster * currCluster = clusterlist->head;
	Clusternode * currNode = currCluster->head;

	printf("\n\n# clusters: %d\n", clusterlist->length);
	for(int i=0; i<clusterlist->length; i++){
		printf("Cluster %d[%d] length: %d\n", i, currCluster, currCluster->length);

		for(int j=0; j<currCluster->length; j++){
			printf("node address: %d\n", currNode);
			currNode = currNode->next;
		}

		if(currCluster->next == 0)
			printf("end of last cluster\n");
		else{
			printf("__cluster iterated. old[%d], new[%d]\n", currCluster, currCluster->next);
			currCluster = currCluster->next;
			printf("__node reset. old[%d], new[%d]\n", currNode, currCluster->head);
			currNode = currCluster->head;
		}
	}
}

Clusterlist * dfs(Node lattice[][NCOLS]){
	Clusterlist * clusterlist = newClusterlist();

	for(int i=0; i<NROWS; i++){
		for(int j=0; j<NCOLS; j++){
			if(lattice[i][j].seen == 0){
				Cluster * cluster = newCluster();
				searchNode(lattice,i,j,0,cluster);	
				addClusterToClusterlist(clusterlist, cluster);
			}
		}
	}

	return clusterlist;
}

void searchNode(Node lattice[][NCOLS], int row, int col, int depth, Cluster * cluster){
	Node currNode = lattice[row][col];
	if(currNode.seen == 0){
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

		// check Node and update cluster
		currNode.seen = true;
		Clusternode * clusternode = newClusternode(row,col);
		addNodeToCluster(cluster,clusternode);

		// put checked Node into lattice
		lattice[row][col] = currNode;
		if(currNode.south == 1){
			searchNode(lattice, nextRow, col, depth, cluster);
		}
		if(currNode.west == 1){
			searchNode(lattice, row, prevCol, depth, cluster);
		}
		if(currNode.east == 1){
			searchNode(lattice, row, nextCol, depth, cluster);
		}
		if(currNode.north == 1){
			searchNode(lattice, prevRow, col, depth, cluster);
		}
	}
}

Clusterlist * newClusterlist(){
	Clusterlist * newClusterlist = malloc(sizeof(Clusterlist));
	newClusterlist->head = NULL;
	newClusterlist->tail = NULL;
	newClusterlist->length = 0;

	return newClusterlist;
}

void addClusterToClusterlist(Clusterlist * clusterlist, Cluster * cluster){
	if(clusterlist->length == 0){
		clusterlist->head = cluster;
	}
	else{
		clusterlist->tail->next = cluster;
		cluster->prev = clusterlist->tail;
	}
	clusterlist->tail = cluster;
	clusterlist->length++;
}

Cluster * newCluster(){
	Cluster * newCluster = malloc(sizeof(Cluster));
	// maybe calloc all these??
	// shouldn't need to since when a cluster or node is created it creates space for itself
	newCluster->head = NULL;
	newCluster->tail = NULL;
	newCluster->prev = NULL;
	newCluster->next = NULL;
	newCluster->length = 0;

	return newCluster;
}

void addNodeToCluster(Cluster * cluster, Clusternode * clusternode){
	if(cluster->length == 0){
		cluster->head = clusternode;
	}
	else{
		// last node and new node point to each other
		cluster->tail->next = clusternode;
		clusternode->prev = cluster->tail;
	}
	cluster->tail = clusternode;
	cluster->length++;
}

Clusternode * newClusternode(int row, int col){
	Clusternode * newClusternode = malloc(sizeof(Clusternode));
	newClusternode->prev = NULL;
	newClusternode->next = NULL;
	newClusternode->row = row;
	newClusternode->col = col;

	return newClusternode;
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
				printf("invalid lattice\n");
				return false;
			if(lattice[i][j].south != lattice[nexti][j].north)
				printf("invalid lattice\n");
				return false;
			if(lattice[i][j].west != lattice[i][prevj].east)
				printf("invalid lattice\n");
				return false;
			if(lattice[i][j].east != lattice[i][nextj].west)
				printf("invalid lattice\n");
				return false;
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
	if(randomn > P){
		return true;
	}
	return false;
}