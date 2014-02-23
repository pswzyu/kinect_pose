#include "predictor.h"
#include <armadillo>
#include "settings.h"
#include "factor.h"
#include <cmath>
#include "utils.h"
#include "inference.h"
#include <string>
#include <fstream>
#include "gaussianofjoint.h"

using namespace arma;
using namespace std;

Predictor::Predictor()
{
}

int Predictor::loadModels()
{
    graph << 0 << 0 << endr << 1 << 0 << endr << 1 << 0 << endr << 1 << 2 << endr
        << 1 << 0 << endr << 1 << 4 << endr << 1 << 0 << endr << 1 << 6 << endr
        << 1 << 0 << endr << 1 << 8 << endr ;

    string line;
    ifstream model_file ("modelsr3.txt");
    if (model_file.is_open())
    {
        // get the meta data
        getline(model_file, line);
        vector<double> meta_data = Utils::split_vec(line, ',');
        int NM = (int)meta_data[0];
        int NP = (int)meta_data[1];
        int NC = (int)meta_data[2];
        n_hidden_card = NC;

        // since every component of the gaussian model will be modified every time, we do not need
        // to use new gaussian mode every iteration
        GaussianOfJoint tmp_gaussian;
        tmp_gaussian.theta = zeros<mat>(NC, 12); // 12 is the number of gaussian linear model parameters
            // y,x,angle depend on parent y,x,angle and a bias term

        // read the models
        for (int step = 0; step != NM; step ++)
        {
            // action model contain a vector , so we have to use new ones every iteration
            ActionModel tmp_model;
            tmp_model.transMatrix = zeros<mat>(NC, NC);
            // the first line should be "="
            getline(model_file, line);
            // second line should be P.c
            getline(model_file, line);
            tmp_model.c = vec( Utils::split_vec( line, ',' ) );
            // P.clg

            for (int step2 = 0; step2 != NP; step2 ++)
            {
                // P.clg.mu_y
                getline(model_file, line);
                if (line.compare("n") != 0)
                {
                    tmp_gaussian.mu_y = vec( Utils::split_vec( line, ',' ) );
                }
                // P.clg.sigma_y
                getline(model_file, line);
                tmp_gaussian.sigma_y = vec( Utils::split_vec( line, ',' ) );
                // P.clg.mu_x
                getline(model_file, line);
                if (line.compare("n") != 0)
                {
                    tmp_gaussian.mu_x = vec( Utils::split_vec( line, ',' ) );
                }
                // P.clg.sigma_x
                getline(model_file, line);
                tmp_gaussian.sigma_x = vec( Utils::split_vec( line, ',' ) );
                // P.clg.mu_angle
                getline(model_file, line);
                if (line.compare("n") != 0)
                {
                    tmp_gaussian.mu_angle = vec( Utils::split_vec( line, ',' ) );
                }
                // P.clg.sigma_angle
                getline(model_file, line);
                tmp_gaussian.sigma_angle = vec( Utils::split_vec( line, ',' ) );

                // P.clg.theta
                for (int step3 = 0; step3 != NC; step3 ++)
                {
                    getline(model_file, line);
                    // no theta
                    if (line.compare("n") == 0)
                    {
                        break;
                    }else
                    {
                        tmp_gaussian.theta.row(step3) = vec( Utils::split_vec( line, ',' ) ).t();
                    }
                }

                tmp_model.clg.push_back( tmp_gaussian );
            }
            // P.transMatrix
            for (int step2 = 0; step2 != NC; step2 ++)
            {
                getline(model_file, line);
                // no transmatrix
                if (line.compare("n") == 0)
                {
                    break;
                }else
                {
                    tmp_model.transMatrix.row(step2) = vec( Utils::split_vec( line, ',' ) ).t();
                }
            }

            models.push_back(tmp_model);
        }


        model_file.close();
    }else
    {
        cout << "Unable to open file";
    }

    return 0;
}

mat Predictor::Predict( std::deque<arma::mat> ps_d )
{
    // convert deque to a vector
    vector<arma::mat> ps;
    for ( deque<arma::mat>::iterator iter = ps_d.begin();
          iter != ps_d.end(); iter ++ )
    {
        ps.push_back( *iter );
    }

    int NA = 1; // number of action that is need to be recog
    int NP = ps.size(); // total number of poses
    int TA = models.size(); // number of type of actions
    int TP = n_hidden_card; // number of type of poses
    mat likelihoods = zeros<mat>(NA, TA);
    int K = TP; // number of pose types

    // can't predict if there are less than 10 frames
    if (ps.size() < 10)
    {
        return zeros<mat>(1, TA);
    }


    for ( int step_ta = 0; step_ta != TA; step_ta ++)
    {
        // calculate the singleton probabilities
        ActionModel& P = models[step_ta];
        //loglikelihood = models(step_ta).loglikelihood;
        //ClassProb = models(step_ta).ClassProb;
        //PairProb = models(step_ta).PairProb;

        mat logEmissionProb = zeros<mat>(NP,TP);

        for (int step1 = 0; step1 != NP; step1 ++)
        {
            //double temp_prob1 = 0;
            for (int k = 0; k != K; k++)   //% all
            {
                double temp_prob2 = 0;

                for (int step2 = 0; step2 != 10; step2 ++)
                {
                    if ( graph(step2, 0) == 0 )  //% no parent
                    {
                        temp_prob2 = temp_prob2 +
                            Utils::lognormpdf( ps[step1](step2,0),P.clg[step2].mu_y(k),P.clg[step2].sigma_y(k) ) +
                            Utils::lognormpdf( ps[step1](step2,1),P.clg[step2].mu_x(k),P.clg[step2].sigma_x(k) ) +
                            Utils::lognormpdf( ps[step1](step2,2),P.clg[step2].mu_angle(k),P.clg[step2].sigma_angle(k) ) ;
                    }else{
                        vec pa;
                        pa << 1 << ps[step1](graph(step2,1),0) << ps[step1](graph(step2,1),1) << ps[step1](graph(step2,1),2) ;
                        temp_prob2 = temp_prob2 +
                            Utils::lognormpdf( ps[step1](step2,0), sum(P.clg[step2].theta.submat(k,0,k,3)*pa), P.clg[step2].sigma_y(k)) +
                            Utils::lognormpdf( ps[step1](step2,1), sum(P.clg[step2].theta.submat(k,4,k,7)*pa), P.clg[step2].sigma_x(k)) +
                            Utils::lognormpdf( ps[step1](step2,2), sum(P.clg[step2].theta.submat(k,8,k,11)*pa), P.clg[step2].sigma_angle(k)) ;
                    }
                }


                logEmissionProb(step1,k) = temp_prob2;
            }
        }

        //% unroll the matrix to a vactor to fit for the val field
        mat pair_factor_val = vectorise(P.transMatrix).t();
        for (int step = 0; step != K*K ; step ++)
        {
            pair_factor_val(0,step) = log( pair_factor_val(0,step) );
        }

        //% build the factors
        // the name of the variables start from 1 !!!!!!!!!!
        for (int step = 0; step != NA; step ++)
        {
            int num_pose = NP;
            int num_pair = NP-1;
            Factor tmp_factor;
            vector<Factor> single_factors(num_pose+1, tmp_factor);
            vector<Factor> pair_factors(num_pair, tmp_factor);

            single_factors[0].var << 1;
            single_factors[0].card << K;
            single_factors[0].val = P.c; // first initialize the vec
            // then convert to log space
            for (int step2 = 0; step2 != P.c.n_rows; step2 ++)
            {
                single_factors[0].val(step2) = log( P.c(step2) );
            }

            for (int step2 = 0; step2 != num_pose; step2 ++)
            {
                single_factors[step2+1].var << step2+2;
                single_factors[step2+1].card << K;
                single_factors[step2+1].val = logEmissionProb.row(step2).t();
            }
            for (int step2 = 0; step2 != num_pair; step2 ++)
            {
                pair_factors[step2].var << step2+2 << step2+3;
                pair_factors[step2].card << K << K;
                pair_factors[step2].val = pair_factor_val.row(0).t();
            }

            // concatenate the single_factors and pair_factors
            for ( vector<Factor>::iterator iter = pair_factors.begin();
                  iter != pair_factors.end(); iter ++ )
            {
                single_factors.push_back( *iter );
            }

            CliqueTree PCalibrated = Inference::ComputeExactMarginalsHMM( single_factors );

            // these are only need in EM
            // for (int step2 = 0; step2 != num_pose; step2 ++)
            // {
            //     ClassProb( datasetTest.actionData(step).marg_ind(step2),: ) = exp( M(step2).val );
            // }
            // for (int step2 = 0; step2 != num_pair; step2 ++)
            // {
            //     //tmp = FactorSum(M(step2),M(step2+1));
            //     //PairProb( actionData(step).pair_ind(step2),: ) = exp(tmp.val);
            //     PairProb(datasetTest.actionData(step).pair_ind(step2),:) = exp(PCalibrated.cliqueList(step2).val - logsumexp(PCalibrated.cliqueList(step2).val) );
            // }
            likelihoods(step,step_ta) += Utils::logsumexp(PCalibrated.cliqueList[1].val);
        }
    }
    return likelihoods;
}
