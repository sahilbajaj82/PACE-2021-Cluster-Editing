# CoRBeC: Cluster Editing PACE 2021 (Heuristic Track)

This repository contains a heuristic solution to the cluster editing problem.

The program is submitted to the [PACE Challenge 2021](https://pacechallenge.org/2021/).

The cluster editing problem consists of removing or inserting as few edges to an undirected graph to obtain a graph where every connected component is a clique.

## Compilation & Usage

The whole program is contained in a single C++ file with no dependencies beyond the STL. 

You can use it as follows:

```
g++ -O2 corbec.cpp -o corbec
./corbec
```

## Description
This solver first identifies all the conflict triples in the given graph and removes them. We then associste a cost with each node that defines the suitability of that node for a cluster formed after conflict reduction in the previous step. We put the node in its best suited cluster as per the cost.
