Clusterlist * dfs(Node lattice[][NCOLS]);
void searchNode(Node lattice[][NCOLS], int row, int col, int depth, Cluster *);
bool clustersPercolateH(Clusterlist *);
bool clustersPercolateV(Clusterlist *);
int getLargestCluster(Clusterlist *);