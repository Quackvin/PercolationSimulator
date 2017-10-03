#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NCOLS 10
#define NROWS 10
#define PRECISION 5

int clusterCount = 0;

typedef struct ClusterData clusterData_t;
typedef struct ClusterStack cstack_t;
typedef struct Cluster cluster_t;
typedef struct Node node_t;

struct ClusterData{
	int size;
	bool percV[NROWS];
	bool percH[NCOLS];
} ;

struct ClusterStack{
	cluster_t *cluster;
	cstack_t next;
} ;

struct Cluster {
    int node_count;
    node_t *nodeHead;
    cstack_t *neighbourHead;
    cluster_t *next;
    bool seen;
} ;

struct Node {
    int x;
    int y;
    node_t *next;
} ;

clusterData_t *joinClusters(cluster_t *head, char *percType, int percDir);
void printClusters(cluster_t *head);
cluster_t *generateCluster(int nodePositions[][2], int numNodes);
node_t *generateNode(int x, int y);
cluster_t *addClusterToList(cluster_t *head, cluster_t *newCluster);
void addNodeToCluster(cluster_t *cluster, node_t *newNode);

int main(int argc, char *argv[]) {
	int numNodes;

	// Create set of test clusters
	cluster_t *head = NULL;

	int c1[][2] = {{1,0},{2,0}};
	numNodes = sizeof(c1)/(sizeof(int)*2);
	head = addClusterToList(head, generateCluster(c1, numNodes));

	int c2[][2] = {{0,1},{1,1}};
	numNodes = sizeof(c2)/(sizeof(int)*2);
	head = addClusterToList(head, generateCluster(c2, numNodes));

	int c3[][2] = {{2,2},{0,2}};
	numNodes = sizeof(c3)/(sizeof(int)*2);
	head = addClusterToList(head, generateCluster(c3, numNodes));

	printClusters(head);

	// connect them
	

    return 0;
}

clusterData_t *joinClusters(cluster_t *head, char *percType, int percDir){
	cstack_t *clusterStack = NULL;
	cluster_t *clustersInOrder = head;

	// look through clusters in order
	while(clustersInOrder){
		// save data for this cluster
		clusterData_t *newData = malloc(sizeof(clusterData_t));
		newData->size = 0;
		for(int i=0; i<NCOLS; i++){
			newData->percV[i] = false;
			newData->percH[i] = false;
		}
		
		// add to stack
		pushCStack(*clusterStack, clustersInOrder);
		while(clusterStack != NULL){

			cluster_t *currClust = popCStack(*clusterStack);
			node_t *currNode = currClust->nodeHead;
			
			if(!(currClust->seen)){
				currClust->seen = true;
				while(currNode){
					newData->size ++;
					newData->percH[currNode->x] = true;
					newData->percV[currNode->y] = true;

					node_t *tempNode = currNode;
					currNode = tempNode->next;
					free(tempNode);
				}
				// add all neighbours to cstack
				cstack_t *currNeighbour = currClust->neighbourHead;
				while(currNeighbour){
					pushCStack(*clusterStack, currNeighbour->cluster);
					cstack_t *tempCStack = currNeighbour;
					currNeighbour = tempCStack->next;
					free(tempCStack);
				}
			}

			// one cluster now completely connected
		}
		clustersInOrder = clustersInOrder->next;
	}

	return newData;
}

void *pushCStack(cstack_t **top, cluster_t *cluster){
	cstack_t *newlink = malloc(sizeof(cstack_t));
	newlink->next = *top;
	newlink->cluster = cluster;
	*top = newlink;
}

cluster_t *popCStack(cstack_t **top){
	cstack_t *temp = (*top)->next;
	cluster_t *cluster = (*top)->cluster;
	free(*top);
	*top = temp;

	return cluster;
}

// everything below is just for testing
void printClusters(cluster_t *head){
	cluster_t *currClust = head;
	while(currClust){
		printf("Cluster %p\n", currClust);

		node_t *currNode = currClust->nodeHead;
		while(currNode){
			printf("x %d y %d\n", currNode->x, currNode->y);
			currNode = currNode->next;
		}
		currClust = currClust->next;
	}
}

cluster_t *generateCluster(int nodePositions[][2], int numNodes){
	cluster_t *newCluster = malloc(sizeof(cluster_t));
	// define random size, generate that many nodes
	// tell node where to be so they are together
	
	newCluster->node_count = 0;
	newCluster->next = NULL;
	newCluster->neighbourHead = NULL;
	newCluster->nodeHead = NULL;
	newCluster->seen = false;

	for(int i=0; i<numNodes; i++){
		addNodeToCluster(newCluster, generateNode(nodePositions[i][0], nodePositions[i][1]));
	}

	return newCluster;
}

node_t *generateNode(int x, int y){
	node_t *newNode = malloc(sizeof(node_t));

	newNode->x = x;
	newNode->y = y;
	newNode->next = NULL;

	return newNode;
}

cluster_t *addClusterToList(cluster_t *head, cluster_t *newCluster){
	newCluster->next = head;
	return newCluster;
}

void addNodeToCluster(cluster_t *cluster, node_t *newNode){
	newNode->next = cluster->nodeHead;
	cluster->nodeHead = newNode;
}





