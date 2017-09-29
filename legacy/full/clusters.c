#include <stdlib.h>

#define DEBUG 0

typedef enum{false,true} bool;

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
	bool percolatesH;
	bool percolatesV;
	int rows[NROWS];
	int cols[NCOLS];
	int rowsSeen;
	int colsSeen;
}Cluster;

typedef struct Clusterlist{
	Cluster * head;
	Cluster * tail;
	int length;
	int largestCluster;
	bool percolatesH;
	bool percolatesV;
}Clusterlist;

Clusterlist * newClusterlist(){
	Clusterlist * newClusterlist = malloc(sizeof(Clusterlist));
	newClusterlist->head = NULL;
	newClusterlist->tail = NULL;
	newClusterlist->length = 0;
	newClusterlist->largestCluster = 0;
	newClusterlist->percolatesH = false;
	newClusterlist->percolatesV = false;

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
	newCluster->head = NULL;
	newCluster->tail = NULL;
	newCluster->prev = NULL;
	newCluster->next = NULL;
	newCluster->length = 0;
	newCluster->percolatesH = false;
	newCluster->percolatesV = false;
	newCluster->rowsSeen = 0;
	newCluster->colsSeen = 0;
	for(int i=0; i<fmax(NROWS,NCOLS); i++){
		if(i<NROWS)
			newCluster->rows[i] = 0;
		if(i<NCOLS)
			newCluster->cols[i] = 0;
	}

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

	// mark row and column as seen
	if(cluster->rows[clusternode->row] == 0){
		cluster->rows[clusternode->row] = 1;
		cluster->rowsSeen++;
	}
	if(cluster->cols[clusternode->col] == 0){
		cluster->cols[clusternode->col] = 1;
		cluster->colsSeen++;
	}

	if(cluster->rowsSeen == NROWS)
		cluster->percolatesV = true;
	if(cluster->colsSeen == NCOLS)
		cluster->percolatesH = true;
}

Clusternode * newClusternode(int row, int col){
	Clusternode * newClusternode = malloc(sizeof(Clusternode));
	newClusternode->prev = NULL;
	newClusternode->next = NULL;
	newClusternode->row = row;
	newClusternode->col = col;

	return newClusternode;
}