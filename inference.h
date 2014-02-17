#ifndef INFERENCE_H
#define INFERENCE_H

#include <deque>
#include <armadillo>
#include <vector>
#include "actionmodel.h"
#include "cliquetree.h"

class Inference
{
public:
	Inference();
	static CliqueTree ComputeExactMarginalsHMM( vector<Factor> factors );
	static CliqueTree CliqueTreeCalibrate( CliqueTree tree );
	static CliqueTree CreateCliqueTreeHMMFast( vector<Factor> factors );
    static Factor FactorMarginalization ( Factor, vec );
};



#endif // INFERENCE_H
