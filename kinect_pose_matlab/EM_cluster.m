% File: EM_cluster.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [P loglikelihood ClassProb] = EM_cluster(poseData, G, InitialClassProb, maxIter)

% INPUTS
% poseData: N x 10 x 3 matrix, where N is number of poses;
%   poseData(i,:,:) yields the 10x3 matrix for pose i.
% G: graph parameterization as explained in PA8
% InitialClassProb: N x K, initial allocation of the N poses to the K
%   classes. InitialClassProb(i,j) is the probability that example i belongs
%   to class j
% maxIter: max number of iterations to run EM

% OUTPUTS
% P: structure holding the learned parameters as described in the PA
% loglikelihood: #(iterations run) x 1 vector of loglikelihoods stored for
%   each iteration
% ClassProb: N x K, conditional class probability of the N examples to the
%   K classes in the final iteration. ClassProb(i,j) is the probability that
%   example i belongs to class j

% Initialize variables
N = size(poseData, 1);
K = size(InitialClassProb, 2);

ClassProb = InitialClassProb;

loglikelihood = zeros(maxIter,1);

P.c = [];
P.clg.sigma_x = [];
P.clg.sigma_y = [];
P.clg.sigma_angle = [];

% EM algorithm
for iter=1:maxIter
  
  % M-STEP to estimate parameters for Gaussians
  %
  % Fill in P.c with the estimates for prior class probabilities
  % Fill in P.clg for each body part and each class
  % Make sure to choose the right parameterization based on G(i,1)
  %
  % Hint: This part should be similar to your work from PA8
  
  P.c = zeros(1,K);
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    P.c = sum(ClassProb) / N;
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
  
  % E-STEP to re-estimate ClassProb using the new parameters
  %
  % Update ClassProb with the new conditional class probabilities.
  % Recall that ClassProb(i,j) is the probability that example i belongs to
  % class j.
  %
  % You should compute everything in log space, and only convert to
  % probability space at the end.
  %
  % Tip: To make things faster, try to reduce the number of calls to
  % lognormpdf, and inline the function (i.e., copy the lognormpdf code
  % into this file)
  %
  % Hint: You should use the logsumexp() function here to do
  % probability normalization in log space to avoid numerical issues
  
  ClassProb = zeros(N,K);
  
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

            ClassProb(step1,k) = temp_prob2 + log(P.c(k));
        end
    end
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % Compute log likelihood of dataset for this iteration
  % Hint: You should use the logsumexp() function here
  loglikelihood(iter) = 0;
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % use the unnormalized classprob to calculate the loglikelihood
  loglikelihood(iter) = sum(logsumexp(ClassProb));
  
  for step = 1:N
      %%% normallize
      ClassProb(step,:) = exp(ClassProb(step,:) - logsumexp( ClassProb(step,:) ));
  end
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % Print out loglikelihood
  disp(sprintf('EM iteration %d: log likelihood: %f', ...
    iter, loglikelihood(iter)));
  if exist('OCTAVE_VERSION')
    fflush(stdout);
  end
  
  % Check for overfitting: when loglikelihood decreases
  if iter > 1
    if loglikelihood(iter) < loglikelihood(iter-1)
      break;
    end
  end
  
end

% Remove iterations if we exited early
loglikelihood = loglikelihood(1:iter);
