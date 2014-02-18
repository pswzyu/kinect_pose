% File: EM_HMM.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [P loglikelihood ClassProb PairProb] = EM_HMM(actionData, poseData, G, InitialClassProb, InitialPairProb, maxIter)

% INPUTS
% actionData: structure holding the actions as described in the PA
% poseData: N x 10 x 3 matrix, where N is number of poses in all actions
% G: graph parameterization as explained in PA description
% InitialClassProb: N x K matrix, initial allocation of the N poses to the K
%   states. InitialClassProb(i,j) is the probability that example i belongs
%   to state j.
%   This is described in more detail in the PA.
% InitialPairProb: V x K^2 matrix, where V is the total number of pose
%   transitions in all HMM action models, and K is the number of states.
%   This is described in more detail in the PA.
% maxIter: max number of iterations to run EM

% OUTPUTS
% P: structure holding the learned parameters as described in the PA
% loglikelihood: #(iterations run) x 1 vector of loglikelihoods stored for
%   each iteration
% ClassProb: N x K matrix of the conditional class probability of the N examples to the
%   K states in the final iteration. ClassProb(i,j) is the probability that
%   example i belongs to state j. This is described in more detail in the PA.
% PairProb: V x K^2 matrix, where V is the total number of pose transitions
%   in all HMM action models, and K is the number of states. This is
%   described in more detail in the PA.

% Initialize variables
N = size(poseData, 1);
K = size(InitialClassProb, 2);
L = size(actionData, 2); % number of actions
V = size(InitialPairProb, 1);

ClassProb = InitialClassProb;
PairProb = InitialPairProb;

loglikelihood = zeros(maxIter,1);

P.c = [];
P.clg.sigma_x = [];
P.clg.sigma_y = [];
P.clg.sigma_angle = [];

% EM algorithm
for iter=1:maxIter
  
  % M-STEP to estimate parameters for Gaussians
  % Fill in P.c, the initial state prior probability (NOT the class probability as in PA8 and EM_cluster.m)
  % Fill in P.clg for each body part and each class
  % Make sure to choose the right parameterization based on G(i,1)
  % Hint: This part should be similar to your work from PA8 and EM_cluster.m
  
  P.c = zeros(1,K);
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
  
    for step = 1:L
        P.c = P.c + ClassProb( actionData(step).marg_ind(1),: );
    end
    P.c = P.c / sum(P.c);
    
    V = size(G,1);

    clg_structure = struct('mu_y',[],'sigma_y',[],'mu_x',[],'sigma_x',[],...
        'mu_angle',[],'sigma_angle',[],'theta',[]);
    P.clg = repmat(clg_structure, 1, V);

    for step = 1:V
        for class = 1:K
            cdata = poseData;
            if G(step,1) == 0 % if the only parent is class
                [P.clg(step).mu_y(class), P.clg(step).sigma_y(class)] = FitG(squeeze(cdata(:,step,1)),ClassProb(:,class));
                [P.clg(step).mu_x(class), P.clg(step).sigma_x(class)] = FitG(squeeze(cdata(:,step,2)),ClassProb(:,class));
                [P.clg(step).mu_angle(class), P.clg(step).sigma_angle(class)] = FitG(squeeze(cdata(:,step,3)),ClassProb(:,class));
            else
                pa = squeeze( cdata(:,G(step,2),:) );
                [theta_y,P.clg(step).sigma_y(class)] = FitLG(squeeze(cdata(:,step,1)),pa,ClassProb(:,class));
                P.clg(step).theta(class,2:4) = theta_y(1:3);
                P.clg(step).theta(class,1) = theta_y(4);
                [theta_x,P.clg(step).sigma_x(class)] = FitLG(squeeze(cdata(:,step,2)),pa,ClassProb(:,class));
                P.clg(step).theta(class,6:8) = theta_x(1:3);
                P.clg(step).theta(class,5) = theta_x(4);
                [theta_angle,P.clg(step).sigma_angle(class)] = FitLG(squeeze(cdata(:,step,3)),pa,ClassProb(:,class));
                P.clg(step).theta(class,10:12) = theta_angle(1:3);
                P.clg(step).theta(class,9) = theta_angle(4);
            end
        end
    end
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % M-STEP to estimate parameters for transition matrix
  % Fill in P.transMatrix, the transition matrix for states
  % P.transMatrix(i,j) is the probability of transitioning from state i to state j
  P.transMatrix = zeros(K,K);
  
  % Add Dirichlet prior based on size of poseData to avoid 0 probabilities
  P.transMatrix = P.transMatrix + size(PairProb,1) * .05;
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  labels = zeros(N,K);
  for step = 1:N
      [val, pos] = max(ClassProb(step,:));
      labels(step,pos) = 1;
  end
  
  P.transMatrix = P.transMatrix + reshape( sum(PairProb), K, K );
  
  for step = 1:K
      P.transMatrix(step,:) = P.transMatrix(step, :) / sum( P.transMatrix(step, :) );
  end
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    
  % E-STEP preparation: compute the emission model factors (emission probabilities) in log space for each 
  % of the poses in all actions = log( P(Pose | State) )
  % Hint: This part should be similar to (but NOT the same as) your code in EM_cluster.m
  
  logEmissionProb = zeros(N,K);
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    dataset = poseData;
    for step1 = 1:N
        temp_prob1 = 0;
        graph = G;
        for k = 1:K % all
            
            temp_prob2 = 0;

            for step2 = 1:10
                if graph(step2, 1) == 0 % no parent
                    temp_prob2 = temp_prob2 + ...
                        lognormpdf(dataset(step1,step2,1),P.clg(step2).mu_y(k),P.clg(step2).sigma_y(k)) + ...
                        lognormpdf(dataset(step1,step2,2),P.clg(step2).mu_x(k),P.clg(step2).sigma_x(k)) + ...
                        lognormpdf(dataset(step1,step2,3),P.clg(step2).mu_angle(k),P.clg(step2).sigma_angle(k)) ;
                else
                    pa = [1, dataset(step1,graph(step2,2),1), dataset(step1,graph(step2,2),2), dataset(step1,graph(step2,2),3)];
                    temp_prob2 = temp_prob2 + ...
                        lognormpdf(dataset(step1,step2,1), pa*P.clg(step2).theta(k,1:4)', P.clg(step2).sigma_y(k)) + ...
                        lognormpdf(dataset(step1,step2,2), pa*P.clg(step2).theta(k,5:8)', P.clg(step2).sigma_x(k)) + ...
                        lognormpdf(dataset(step1,step2,3), pa*P.clg(step2).theta(k,9:12)', P.clg(step2).sigma_angle(k)) ;
                end
            end

            logEmissionProb(step1,k) = temp_prob2;
        end
    end
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    
  % E-STEP to compute expected sufficient statistics
  % ClassProb contains the conditional class probabilities for each pose in all actions
  % PairProb contains the expected sufficient statistics for the transition CPDs (pairwise transition probabilities)
  % Also compute log likelihood of dataset for this iteration
  % You should do inference and compute everything in log space, only converting to probability space at the end
  % Hint: You should use the logsumexp() function here to do probability normalization in log space to avoid numerical issues
  
  ClassProb = zeros(N,K);
  PairProb = zeros(V,K^2);
  loglikelihood(iter) = 0;
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % unroll the matrix to a vactor to fit for the val field
  pair_factor_val = zeros(1, K*K);
  asgn = IndexToAssignment( 1:K*K, [K,K]);
  for step = 1:K*K
      pair_factor_val(1,step) = log(P.transMatrix( asgn(step,1), asgn(step,2) ));
  end
  
  % build the factors
  for step = 1:L
      num_pose = length(actionData(step).marg_ind);
      num_pair = length(actionData(step).pair_ind);
      single_factors = repmat(struct('var',[],'card',[],'val',[]), 1,num_pose+1);
      pair_factors = repmat(struct('var',[],'card',[],'val',[]), 1,num_pair);
      
      single_factors(1).var = [1];
      single_factors(1).card = [K];
      single_factors(1).val = log(P.c);
      for step2 = 1:num_pose
          single_factors(step2+1).var = [step2];
          single_factors(step2+1).card = [K];
          single_factors(step2+1).val = logEmissionProb(actionData(step).marg_ind(step2),:);
      end
      for step2 = 1:num_pair
          pair_factors(step2).var = [step2, step2+1];
          pair_factors(step2).card = [K,K];
          pair_factors(step2).val = pair_factor_val;
      end
      
      [M, PCalibrated] = ComputeExactMarginalsHMM([single_factors, pair_factors]);
      
      for step2 = 1:num_pose
          ClassProb( actionData(step).marg_ind(step2),: ) = exp( M(step2).val - logsumexp(M(step2).val) );
      end
      for step2 = 1:num_pair
          %tmp = FactorSum(M(step2),M(step2+1));
          %PairProb( actionData(step).pair_ind(step2),: ) = exp(tmp.val);
          PairProb(actionData(step).pair_ind(step2),:) = exp(PCalibrated.cliqueList(step2).val - logsumexp(PCalibrated.cliqueList(step2).val) );
      end
      loglikelihood(iter) = loglikelihood(iter) + logsumexp(PCalibrated.cliqueList(1).val);
  end
  
  
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % Print out loglikelihood
  disp(sprintf('EM iteration %d: log likelihood: %f', ...
    iter, loglikelihood(iter)));
  if exist('OCTAVE_VERSION')
    fflush(stdout);
  end
  
  % Check for overfitting by decreasing loglikelihood
  if iter > 1
    if loglikelihood(iter) < loglikelihood(iter-1)
      break;
    end
  end
  
end

% Remove iterations if we exited early
loglikelihood = loglikelihood(1:iter);
