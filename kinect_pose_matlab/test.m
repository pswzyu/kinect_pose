function testFunc(testc)

load PA9SampleCases


for ii = testc
  if ii == 1 % EM_cluster
 [P loglikelihood ClassProb] = EM_cluster(exampleINPUT.t1a1, exampleINPUT.t1a2, exampleINPUT.t1a3, exampleINPUT.t1a4);     
 retval(1) = CompareData(P, exampleOUTPUT.t1a1, [], struct('NumericTolerance',1e-3,'displaycontextprogress',0));
 retval(2) = CompareData(loglikelihood, exampleOUTPUT.t1a2, [], struct('NumericTolerance',1e-2,'displaycontextprogress',0));
 retval(3) = CompareData(ClassProb, exampleOUTPUT.t1a3, [], struct('NumericTolerance',1e-4,'displaycontextprogress',0));
 printout('EM Cluster Test',retval); 
 elseif ii == 2 % EM_HMM - first iteration
 [P loglikelihood ClassProb PairProb] = EM_HMM(exampleINPUT.t2a1, exampleINPUT.t2a2, exampleINPUT.t2a3, exampleINPUT.t2a4, exampleINPUT.t2a5, 2);
 retval(1) = CompareData(P, exampleOUTPUT.t2a1b, [], struct('NumericTolerance',1e-3,'displaycontextprogress',0));
 retval(2) = CompareData(loglikelihood, exampleOUTPUT.t2a2b, [], struct('NumericTolerance',1e-3,'displaycontextprogress',0));
 retval(3) = CompareData(ClassProb, exampleOUTPUT.t2a3b, [], struct('NumericTolerance',1e-5,'displaycontextprogress',0));
 retval(4) = CompareData(PairProb, exampleOUTPUT.t2a4b, [], struct('NumericTolerance',1e-5,'displaycontextprogress',0));
 printout('EM HMM Test - Iteration 1',retval);
 elseif ii == 3 % EM_HMM - second iteration
 [P loglikelihood ClassProb PairProb] = EM_HMM(exampleINPUT.t2a1, exampleINPUT.t2a2, exampleINPUT.t2a3, exampleINPUT.t2a4, exampleINPUT.t2a5, exampleINPUT.t2a6); 
 retval(1) = CompareData(P, exampleOUTPUT.t2a1, [], struct('NumericTolerance',1e-3,'displaycontextprogress',0));
 retval(2) = CompareData(loglikelihood, exampleOUTPUT.t2a2, [], struct('NumericTolerance',1e-3,'displaycontextprogress',0));
 retval(3) = CompareData(ClassProb, exampleOUTPUT.t2a3, [], struct('NumericTolerance',1e-5,'displaycontextprogress',0));
 retval(4) = CompareData(PairProb, exampleOUTPUT.t2a4, [], struct('NumericTolerance',1e-5,'displaycontextprogress',0));
 printout('EM HMM Test - Iteration 2',retval);
 end
end


function printout(txt,retval)
 if all(retval)
     disp(['Success on ' txt]);
 else
     disp(['Fail on ' txt]);
 end