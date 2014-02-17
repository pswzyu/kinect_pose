#include "cliquetree.h"
#include "settings.h"
#include <cmath>

using namespace arma;
using namespace std;

CliqueTree::CliqueTree( int p_nodes )
{
	Factor tmp_factor;
    cliqueList = vector<Factor>(p_nodes, tmp_factor);

	edges = zeros<mat>(p_nodes, p_nodes);
}
