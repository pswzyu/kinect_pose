#ifndef TESTPREDICT_H
#define TESTPREDICT_H

#include <deque>
#include <armadillo>

class TestPredict
{
public:
    TestPredict();
    int test();

    std::deque<arma::mat> getPoses(int num);
};

#endif // TESTPREDICT_H
