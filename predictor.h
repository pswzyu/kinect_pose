#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <deque>
#include <armadillo>
#include <vector>
#include "actionmodel.h"
#include "cliquetree.h"

class Predictor
{
public:
    Predictor();
    int loadModels();
    mat Predict( std::deque<arma::mat> ps ); // predict the type of action of the
        // input data
    std::vector<ActionModel> models;
    int n_hidden_card;
    arma::mat graph;

};

#endif // PREDICTOR_H
