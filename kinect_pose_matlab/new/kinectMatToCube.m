function [ output_args ] = kinect2matlab( input_args )
%KINECT2MATLAB Summary of this function goes here
%   Detailed explanation goes here
% turn matrix that is (10*NP) * 3 to  NP*10*3 cube

NP = size(input_args, 1);

output_args = zeros(NP, 10, 3);

for step = 1:NP
    data = input_args( (1+(step-1)*10):(step*10),: );
    output_args(step, :, :) = data;
end


end

