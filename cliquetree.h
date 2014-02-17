#ifndef CLIQUETREE_H
#define CLIQUETREE_H

#include <deque>
#include <armadillo>
#include <vector>
#include "factor.h"
#include "actionmodel.h"

class CliqueTree
{
public:
    CliqueTree(int p_nodes);
    std::vector<Factor> cliqueList;
    arma::mat edges;

};

#endif // CLIQUETREE_H
