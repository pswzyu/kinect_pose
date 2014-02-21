function [ P, loglikelihood, ClassProb, PairProb ] = learnModels( td, card, n_reinit)
%LEARNMODELS Summary of this function goes here
%   Detailed explanation goes here

G = [0,0;1,1;1,1;1,3;1,1;1,5;1,1;1,7;1,1;1,9];

NP = size( td.poseData, 1 );

best_class_prob = [];
best_pair_prob = [];
best_loglikelihood = -99999999999999;
best_p = [];

for step = 1:n_reinit
<<<<<<< HEAD
    step
=======
>>>>>>> 304216c8cb02419ab1701d4c24621ca09614cc7d
    td.InitialClassProb = rand(NP, card);
    td.InitialPairProb = rand(NP, card*card);
    [P, loglikelihood, ClassProb, PairProb] = EM_HMM(...
        td.actionData, td.poseData, G,...
        td.InitialClassProb, td.InitialPairProb, 10);
    if loglikelihood(end,1) > best_loglikelihood
        best_loglikelihood = loglikelihood(end,1);
        best_class_prob = ClassProb;
        best_pair_prob = PairProb;
        best_p = P;
    end
end

loglikelihood = best_loglikelihood;
ClassProb = best_class_prob;
PairProb = best_pair_prob;
P = best_p;

end

