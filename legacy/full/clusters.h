typedef struct Clusternode;
typedef struct Cluster;
typedef struct Clusterlist;

Clusterlist * newClusterlist();
void addClusterToClusterlist(Clusterlist *, Cluster *);
Cluster * newCluster();
void addNodeToCluster(Cluster *, Clusternode *);
Clusternode * newClusternode(int row, int col);