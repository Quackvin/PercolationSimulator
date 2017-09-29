#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "nodes.h"
#include "clusters.h"

#define DEBUG 0

Clusterlist * dfs(Node lattice[][NCOLS]){
	Clusterlist * clusterlist = newClusterlist();

	for(int i=0; i<NROWS; i++){
		for(int j=0; j<NCOLS; j++){
			if(lattice[i][j].seen == 0){
				Cluster * cluster = newCluster();
				searchNode(lattice,i,j,0,cluster);	
				addClusterToClusterlist(clusterlist, cluster);
				if(DEBUG)
					printf("#####Cluster[%d] size: %d\n#####", clusterlist->length, cluster->length);
			}
		}
	}

	clusterlist->percolatesH = clustersPercolateH(clusterlist);
	clusterlist->percolatesV = clustersPercolateV(clusterlist);
	clusterlist->largestCluster = getLargestCluster(clusterlist);

	if(DEBUG)
		printf("# clusters: %d\n", clusterlist->length);

	return clusterlist;
}

void searchNode(Node lattice[][NCOLS], int row, int col, int depth, Cluster * cluster){
	Node currNode = lattice[row][col];
	if(currNode.seen == 0){
		if(DEBUG){
			printf("\n\n___%d,%d, depth:[%d]\n", row,col,depth);
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

		// check Node and update cluster
		currNode.seen = true;
		Clusternode * clusternode = newClusternode(row,col);
		addNodeToCluster(cluster,clusternode);

		if(DEBUG){
			printf("_____visited node[%d][%d][%d]\n",row,col,clusternode);
		}

		if(DEBUG)
			printLattice(lattice,false,true);

		// put checked Node into lattice
		lattice[row][col] = currNode;
		if(currNode.south == 1){
			if(DEBUG)
				printf(" trigger south\n");
			searchNode(lattice, nextRow, col, depth, cluster);
		}
		if(currNode.west == 1){
			if(DEBUG)
				printf(" trigger west\n");
			searchNode(lattice, row, prevCol, depth, cluster);
		}
		if(currNode.east == 1){
			if(DEBUG)
				printf(" trigger east\n");
			searchNode(lattice, row, nextCol, depth, cluster);
		}
		if(currNode.north == 1){
			if(DEBUG)
				printf(" trigger north\n");
			searchNode(lattice, prevRow, col, depth, cluster);
		}
	}
	else
		if(DEBUG)
			printf("_____skipped\n");
}

bool clustersPercolateH(Clusterlist * clusters){
	Cluster * cluster = clusters->head;
	for(int i=0; i<clusters->length; i++){
		if(cluster->percolatesH)
			return true;
		if(cluster->next != 0)
			cluster = cluster->next;
	}
	return false;
}

bool clustersPercolateV(Clusterlist * clusters){
	Cluster * cluster = clusters->head;
	for(int i=0; i<clusters->length; i++){
		if(cluster->percolatesV)
			return true;
		if(cluster->next != 0)
			cluster = cluster->next;
	}
	return false;
}

int getLargestCluster(Clusterlist * clusters){
	Cluster * cluster = clusters->head;
	int max = 0;
	for(int i=0; i<clusters->length; i++){
		if(cluster->length > max)
			max = cluster->length;
		if(cluster->next != 0)
			cluster = cluster->next;
	}
	return max;
}