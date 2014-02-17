#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <armadillo>
#include <string>
#include <vector>

class Utils
{
public:
    Utils();

    static double lognormpdf( double value, double mu, double sigma );
    static double logsumexp( arma::vec values );

    static arma::vec AssignmentToIndex(arma::mat, arma::vec);
    static arma::mat IndexToAssignment(arma::vec, arma::vec);

    static std::vector<double> split_vec(const std::string &s, char delim);

    static double pi;
};

#endif // UTILS_H
