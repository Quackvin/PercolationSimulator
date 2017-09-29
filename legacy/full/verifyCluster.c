#include <stdio.h>
#include <stdlib.h>

#include "clusters.h"

#define DEBUG 0

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