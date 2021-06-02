# PACE-2021-Cluster-Editing
PACE 2021 Challenge - Cluster Editing ( Heuristic Track)
## How to Use
No requirement for external libraries.
``` 
g++ -O2 final.cpp -o final
./final 
```
## Description
This solver first identifies all the conflict triples in the given graph and removes them. We then associste a cost with each node that defines the suitability of that node for a cluster formed after conflict reduction in the previous step. We put the node in its best suited cluster as per the cost.  
