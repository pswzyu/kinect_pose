#include "testpredict.h"
#include <armadillo>
#include "predictor.h"
#include "actionmodel.h"
#include "utils.h"

using namespace arma;
using namespace std;

TestPredict::TestPredict()
{
}

int TestPredict::test()
{
    Predictor p;
    p.loadModels();

    //p.models[0].clg[1].theta.print();
    //int action = p.Predict(getPoses(17));

    //cout << action << endl;

    return 0;
}

std::deque<arma::mat> TestPredict::getPoses(int num)
{
    deque<mat> result;
    string line;
    ifstream poses_file ("poses.txt");
    mat tmp_mat = zeros<mat>(10, 3);
    if (poses_file.is_open())
    {
        for ( int step = 0; step != num; step ++ )
        {
            for (int step2 = 0; step2 != 10; step2 ++)
            {
                getline(poses_file, line);
                vector<double> point = Utils::split_vec(line, ',');
                tmp_mat.row(step2) = vec(point).t();
            }
            result.push_back(tmp_mat);
        }
        // get the meta data
        // getline(model_file, line);

    }else
    {
        cout << "Unable to open file";
    }

    return result;
}




//for (int haha = 0; haha != 3; haha ++)
//{
//    //models[haha].c.print();
//    //models[haha].transMatrix.print();
//    for (int in = 0; in != 10; in ++)
//    {
//        models[haha].clg[in].mu_y.t().print();
//        models[haha].clg[in].sigma_y.t().print();
//        models[haha].clg[in].mu_x.t().print();
//        models[haha].clg[in].sigma_x.t().print();
//        models[haha].clg[in].mu_angle.t().print();
//        models[haha].clg[in].sigma_angle.t().print();
//        //models[haha].clg[in].theta.print();
//        //models[haha].clg[in].mu_y.print();
//    }
//}
