#include "utils.h"
#include <sstream>
#include <cstdlib>
#include <iostream>

double Utils::pi = atan(1)*4;

Utils::Utils()
{
}

// the log of the probability of a value in a normal distribution
double Utils::lognormpdf( double value, double mu, double sigma )
{
    return -log( sigma*sqrt( 2 * pi ) ) - ( value - mu ) * ( value - mu ) / ( 2 * sigma * sigma);
}

// calculate the log( sum( exp(values) ) ) and avoid numarecal issues
double Utils::logsumexp( arma::vec values )
{
	double result = 0;
	double max_value = values.max();

	for ( int step = 0; step != values.n_rows; step ++)
	{
		result += exp( values(step) - max_value );
	}

	result = max_value + log(result);
    return result;
}

arma::vec Utils::AssignmentToIndex(arma::mat assignments, arma::vec cards)
{
    arma::vec weights( cards.n_rows ); // calculate what 1 means in one position, for example
    // 111 means 100, 10, 1
    for (int step = 0; step != weights.n_rows; step ++)
    {
        if (step == 0)
        {
            weights(step) = 1;
        }else
        {
            weights(step) = weights(step-1) * cards(step-1);
        }
    }
    arma::vec result(assignments.n_rows);
    for (int step = 0; step != assignments.n_rows; step ++)
    {
        result(step) = sum( weights % assignments.row(step).t() );
    }
    return result;
}
arma::mat Utils::IndexToAssignment(arma::vec indexes, arma::vec cards)
{
    arma::vec weights( cards.n_rows ); // calculate what 1 means in one position, for example
    // 111 means 100, 10, 1
    for (int step = 0; step != weights.n_rows; step ++)
    {
        if (step == 0)
        {
            weights(step) = 1;
        }else
        {
            weights(step) = weights(step-1) * cards(step-1);
        }
    }
    arma::mat result(indexes.n_rows, cards.n_rows);
    for (int step = 0; step != indexes.n_rows; step ++)
    {
        int remain = (int)indexes(step);
        for (int step2 = 0; step2 != weights.n_rows; step2 ++)
        {
            result(step,weights.n_rows-1-step2) =
                    floor( remain / weights(weights.n_rows-1-step2) );
            remain = remain % (int)weights(weights.n_rows-1-step2) ;
        }
    }
    return result;
}

std::vector<double> Utils::split_vec(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<double> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back( std::atof(item.c_str()) );
    }
    return elems;
}

nite::Point3f pmp(nite::Point3f first, nite::Point3f second)
{
    nite::Point3f result;
    result.x = first.x - second.x;
    result.y = first.y - second.y;
    result.z = first.z - second.z;

    return result;
}

