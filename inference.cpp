#include "inference.h"
#include <iostream>
#include "utils.h"
#include "cliquetree.h"

//%COMPUTEEXACTMARGINALSHMM Runs exact inference and returns the marginals
//%over all the variables and the calibrated clique tree.

//%   M = COMPUTEEXACTMARGINALSHMM(F) Takes a list of factors F,
//%   and runs exact inference and returns the calibrated clique tree (unnormalized) and
//%   final marginals (normalized) for the variables in the network.
//%   It returns an array of size equal to the number of variables in the
//%   network where M(i) represents the ith variable and M(i).val represents
//%   the marginals of the ith variable.


//%
//% Copyright (C) Daphne Koller, Stanford Univerity, 2012
//% M = repmat(struct('var', 0, 'card', 0, 'val', []), length(N), 1);
//%
//% where N is the number of variables in the network, which can be determined
//% from the factors F.

//% Create a clique tree, compute the initial potentails, calibrate the
//% clique tree, and find the belief for each varaible at a clique that has
//% that variable in its scope


CliqueTree Inference::ComputeExactMarginalsHMM( vector<Factor> factors )
{
    CliqueTree compressedCliqueTree = CreateCliqueTreeHMMFast( factors );

    CliqueTree PCalibrated = CliqueTreeCalibrate( compressedCliqueTree );

    int maxVar = 0;
    for ( vector<Factor>::iterator iter = factors.begin();
        iter != factors.end(); iter ++ )
    {
        double max_now = iter->var.max();
        if ( max_now > maxVar )
        {
            maxVar = max_now;
        }
    }

    vec varsList = linspace<vec>(1, maxVar, maxVar);
    Factor tmp_factor;
    vector<Factor> M;
    for (int step = 0; step != varsList.n_rows; step ++)
    {
        M.push_back( tmp_factor );
    }

    for (int i = 0; i != varsList.n_rows; i ++)
    {
        //assert (varsList(i) == i);
        if ( varsList(i) != i+1 )
        {
            std::cout << "ERROR: var mismatch!" << std::endl;
        }

        Factor clique;
        vec to_sum_out;
        if (i == 0)
        {
            clique = PCalibrated.cliqueList[i];
            to_sum_out << 1;
            M[i] = FactorMarginalization(clique, to_sum_out);
        }else{
            clique = PCalibrated.cliqueList[i-1];
            to_sum_out << i-1;
            M[i] = FactorMarginalization(clique, to_sum_out);
        }

        if ( any(M[i].val != 0) )
        {
            //% Normalize
            M[i].val -= Utils::logsumexp(M[i].val);
        }

    }


    return PCalibrated;

}


//%CLIQUETREECALIBRATE Performs sum-product or max-product algorithm for
//%clique tree calibration.

//%   P = CLIQUETREECALIBRATE(P, isMax) calibrates a given clique tree, P
//%   according to the value of isMax flag. If isMax is 1, it uses max-product
//%   message passing, otherwise uses sum-product. This function
//%   returns the clique tree where the .val for each clique in .cliqueList
//%   is set to the final calibrated potentials.

//%
//% Copyright (C) Daphne Koller, Stanford Univerity, 2012

//function P = CliqueTreeCalibrate(P)

CliqueTree Inference::CliqueTreeCalibrate( CliqueTree P )
{    
    //% number of cliques in the tree.
    int N = P.cliqueList.size();
    int card = P.cliqueList[0].card(0);

    //% setting up the messages that will be passed.
    //MESSAGES = repmat(struct('var', [], 'card', [], 'val', []), N, N);
    vector< vector<Factor> > MESSAGES;
    vector<Factor> tmp_factors;
    for (int step = 0; step != N; step ++)
    {
        tmp_factors.push_back( Factor() );
    }
    for (int step = 0; step != N; step ++)
    {
        MESSAGES.push_back( tmp_factors );
    }

    //% These will make sure that the same message is not passed twice in a row
    //N = length(P.cliqueList);
    // 1234 1234 1234 1234
    mat forwardInds = linspace<mat>( 0, card - 1, card ).t();
    forwardInds = repmat( forwardInds, 1, card );

    // 1111 2222 3333 4444
    mat backInds = zeros<mat>(forwardInds.n_rows, forwardInds.n_cols);

    // %prepare our matrices
    for (int i = 0; i != card; i ++ )
    {
        backInds.cols( i*card, (i+1)*card - 1 ) = ones<mat>(1, card ) * i;
    }
    int lastCliqueOne = -1;
    int lastCliqueTwo = 0;
    int change = 1;

    int varOffset=1;
// here
    while (true)
    {
//        % While there are ready cliques to pass messages between, keep passing
//        % messages. Use GetNextCliques to find cliques to pass messages between.
//        % Once you have clique i that is ready to send message to clique
//        % j, compute the message and put it in MESSAGES(i,j).
//        % Remember for clique tree, you only need an upward pass and a downward
//        % pass.

//        % Find the clique that is ready, compute the message for that clique,
//        % and add that message to the queue

        int cliqueOne = lastCliqueOne + change;
        int cliqueTwo = lastCliqueTwo + change;
        lastCliqueOne = cliqueOne;
        lastCliqueTwo = cliqueTwo;


        if (cliqueTwo == -1)
        {
            //% There are no ready cliques, so stop
            break;
        }

        Factor currentMessage = P.cliqueList[cliqueOne];
        // when cliqueOne is equal to 0 or N-1, this part will not exe, the block
        // after this if clause will marginalize the head and tail of the chain
        // after that, the MESSAGES will contain the message from head *OR* tail,
        // which will be used here
        if ( cliqueOne > 0 && cliqueOne < N-1 ) // pswzyu:seems this is because the first and last
            // clique does not need to pass message
        {
            if (change>0) // forward pass
            {
                for (int step = 0; step != card*card; step ++)
                {
                    currentMessage.val(step) += MESSAGES[cliqueOne-change][cliqueOne].val((int)forwardInds(step));
                }
            }else // backward pass
            {
                for (int step = 0; step != card*card; step ++)
                {
                    currentMessage.val(step) += MESSAGES[cliqueOne-change][cliqueOne].val((int)backInds(step));
                }
            }
        }

        int varsToRemove = cliqueOne + varOffset;

        // % Use FactorMaxMarginalization to get the marginals
        vec to_remove;
        to_remove << varsToRemove;
        MESSAGES[cliqueOne][cliqueTwo] = FactorMarginalization(currentMessage,to_remove);
        // %Reverse if necessary pswzyu :: go backward after reaching the tail
        if (cliqueTwo >= N-1)
        {
            change=-1;
            lastCliqueOne = lastCliqueTwo+1;
            varOffset=2;
        }
    }

//    % Now the clique tree has been calibaarated.
//    % Compute the final potentials for the cliques and place them in P.

//    % Iterate through the incoming messages, multiply them by the initial
//    % potential, and normalize
    for (int step1 = 0; step1 != card*card; step1 ++)
    {
        P.cliqueList[0].val(step1) += MESSAGES[1][0].val(backInds(step1));
        P.cliqueList[N-1].val(step1) += MESSAGES[N-2][N-1].val(forwardInds(step1));
    }

    for ( int i = 1; i != N-1; i ++)
    {
        for (int step = 0; step != card*card; step ++)
        {
            P.cliqueList[i].val(step) += MESSAGES[i+1][i].val(backInds(step));
            P.cliqueList[i].val(step) += MESSAGES[i-1][i].val(forwardInds(step));
        }
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    return P;

}


// % CreateCliqueTreeHMM Takes in a list of factors F and returns a clique
// % tree. Should only be called when F meets the following conditions:
// %
// % 1) Factors are over 1 or 2 variables
// % 2) All 2-variable factors are over variables (i,i+1)
// % 3) All variables have the same cardinality
// %
// % Roughly, these conditions mean that the factors of F define a standard
// % chain model such as an HMM.
// %
// % Function returns a clique tree that has the following fields:
// % - .edges: Contains indices of the nodes that have edges between them.
// % - .factorList: Contains the list of factors used to build the Clique
// %                tree.

// %
// % Copyright (C) Daphne Koller, Stanford Univerity, 2012

CliqueTree Inference::CreateCliqueTreeHMMFast( vector<Factor> factors )
{

    int maxVar = 0;
	// get the max var id
	for ( vector<Factor>::iterator iter = factors.begin();
		iter != factors.end(); iter ++ )
	{
		double max_now = iter->var.max();
		if ( max_now > maxVar )
		{
			maxVar = max_now;
		}
	}
    int numNodes = maxVar - 1;
    int card = factors[0].card(0);

    CliqueTree P(numNodes);
    //P.cliqueList = repmat(struct('var', [], 'card', [], 'val', []), numNodes, 1);
    //P.edges = zeros(numNodes);

	for ( int i = 0; i != numNodes; i++ )
	{
        P.cliqueList[i].var << i+1 << i+2;
        P.cliqueList[i].card << card << card;
        P.cliqueList[i].val = ones<vec>(card * card);
	    
        if (i > 0)
	    {
	        P.edges(i, i-1) = 1;
	        P.edges(i-1, i) = 1;
        }
	}

    // the name of the variable starts from 1 !!!!
    for ( int i = 0; i != factors.size(); i ++ )
	{
        Factor f = factors[i];
        int cliqueIdx = 0;
        if (f.var.n_rows == 1)
	    {
            if (f.var(0) > 1)
	        {
                cliqueIdx = f.var(0) - 1;
	        }else{
                cliqueIdx = 1;
	        }
	        
            vec updateIdxs;
            mat assignments = zeros<mat>(card, 2);
            vec cards;
            cards << card << card;
	        for ( int assignment = 0; assignment != card; assignment ++ )
	        {
               if (f.var(0) == cliqueIdx)
	           {
                   assignments.col(0) = linspace<vec>(0, card - 1, card);
                   assignments.col(1) = assignment*ones<vec>(card);
                   updateIdxs = Utils::AssignmentToIndex(assignments, cards);
	           }else{
                   assignments.col(1) = linspace<vec>(0, card - 1, card);
                   assignments.col(0) = assignment*ones<vec>(card);
                   updateIdxs = Utils::AssignmentToIndex(assignments, cards);
               }
               for (int step = 0; step != updateIdxs.n_rows; step ++)
               {
                   P.cliqueList[cliqueIdx - 1].val(updateIdxs(step)) += f.val(step);
               }
	        }        
	    }else{
            if ( f.var.n_rows != 2 )
            {
                std::cout << "ERROR: var more than 2!" << std::endl;
            }
            cliqueIdx = min(f.var);
            if (f.var(0) > f.var(1))
	        {
	            // % sort the factor val so it's in increasing var order
                uvec order = sort_index(f.var); //%#ok
                mat oldAssignments = Utils::IndexToAssignment(linspace<vec>(0,f.val.n_rows-1,f.val.n_rows), f.card);
                mat newAssignments = oldAssignments;
                // (:, order)
                for (int step = 0; step != oldAssignments.n_cols; step ++)
                {
                    newAssignments.col(step) = oldAssignments.col(order(step));
                }
                vec new_card = f.card;
                for (int step = 0; step != f.card.n_rows; step ++)
                {
                    new_card(step) = f.card(order(step));
                }
                f.card = new_card;
                f.var = sort(f.var);
                vec new_index = Utils::AssignmentToIndex(newAssignments, f.card);
                vec new_val = f.val;
                for ( int step = 0; step != f.val.n_rows; step ++ )
                {
                    new_val(step) = f.val( new_index(step) );
                }
                f.val = new_val;
            }

            P.cliqueList[cliqueIdx - 1].val += f.val;
	    }
	}
    return P;
}

//%   FactorMarginalization Sums given variables out of a factor in log space.
//%   B = FactorMarginalization(A,V) computes the factor with the variables
//%   in V summed out. The factor data structure has the following fields:
//%       .var    Vector of variables in the factor, e.g. [1 2 3]
//%       .card   Vector of cardinalities corresponding to .var, e.g. [2 2 2]
//%       .val    Value table of size prod(.card)
//%
//%   The resultant factor should have at least one variable remaining or this
//%   function will throw an error.
//%
//% Copyright (C) Daphne Koller, Stanford Univerity, 2012

Factor Inference::FactorMarginalization ( Factor A, vec ele)
{
    //function B = FactorMarginalization(A,V)
    // this function is only for hmm, ele has only one var
    // var......
    Factor B;
    // eliminate te first var
    B.var << A.var(1);
    B.card << A.card(0);
    mat Val = A.val.t();
    Val = reshape(Val,B.card(0),B.card(0));

    // eliminate the second var
    if (ele(0) == A.var(1))
    {
        Val = Val.t();
        B.var << A.var(0);
    }

    //B.val = log(sum(exp(bsxfun(@minus, Val, max(Val)))))+max(Val);
    B.val = zeros<vec>(Val.n_cols);
    for (int step = 0; step != Val.n_cols; step ++)
    {
        B.val(step) = Utils::logsumexp( Val.col(step) );
    }

    return B;
}






