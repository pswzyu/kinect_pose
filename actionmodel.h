#ifndef ACTIONMODEL_H
#define ACTIONMODEL_H

#include <armadillo>
#include <vector>

#include "gaussianofjoint.h"

using namespace arma;
using namespace std;

class ActionModel
{
public:
    ActionModel();

    vec c;
    vector<GaussianOfJoint> clg;
    mat transMatrix;

};

#endif // ACTIONMODEL_H
