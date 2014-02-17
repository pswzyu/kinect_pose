#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <armadillo>
#include <string>
#include <vector>
#include <NiTE.h>

class Utils
{
public:
    Utils();

    static double lognormpdf( double value, double mu, double sigma );
    static double logsumexp( arma::vec values );

    static arma::vec AssignmentToIndex(arma::mat, arma::vec);
    static arma::mat IndexToAssignment(arma::vec, arma::vec);

    static std::vector<double> split_vec(const std::string &s, char delim);

    static nite::Point3f pmp(nite::Point3f first, nite::Point3f second);

    static double pi;
};

#endif // UTILS_H
