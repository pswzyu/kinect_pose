#ifndef GAUSSIANOFJOINT_H
#define GAUSSIANOFJOINT_H

#include <armadillo>
using namespace arma;


class GaussianOfJoint
{
public:
    GaussianOfJoint();

    int hidden_card;
    vec mu_y;
    vec sigma_y;
    vec mu_x;
    vec sigma_x;
    vec mu_angle;
    vec sigma_angle;
    mat theta;
};

#endif // GAUSSIANOFJOINT_H
