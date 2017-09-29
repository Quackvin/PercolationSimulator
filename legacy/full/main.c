#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NCOLS 10
#define NROWS 10
#define P 0.5
#define PRECISION 5
#define DEBUG 0

int main(int argc, char* argv[]){
	srand(time(NULL)); 

	static Node lattice[NROWS][NCOLS];
	randomiseLattice(lattice);

	// check generation of bonds is valid
	if(DEBUG)
		checkLattice(lattice);

	// initialise lattice to all nodes unvisited and print a visualisation
	unseeLattice(lattice);

	if(!DEBUG)
		printLattice(lattice,true,false);

	Clusterlist * allClusters = dfs(lattice);

	printf("PercH: %d PercV: %d Largest: %d\n", allClusters->percolatesH, allClusters->percolatesV, allClusters->largestCluster);

	if(DEBUG)
		clustersTest(allClusters);

	if(DEBUG){
		printLattice(lattice,false,true);
	}

	return 0;
}