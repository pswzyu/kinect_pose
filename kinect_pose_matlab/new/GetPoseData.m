function [ output_args ] = GetPoseData( poses, start, stop, filename )
%GETPOSEDATA Summary of this function goes here
%   Detailed explanation goes here
% save the NP*10*3 pose data to file, only used in testpredict
output_args = 1;
for step = start:stop
    dlmwrite( filename, squeeze(poses.poseData(step, :, :)), '-append' );
end

end

