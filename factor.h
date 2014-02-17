#ifndef FACTOR_H
#define FACTOR_H

#include <armadillo>

class Factor
{
public:
    Factor();
    void print();

    arma::vec var;
    arma::vec card;
    arma::vec val;

};

#endif // FACTOR_H
