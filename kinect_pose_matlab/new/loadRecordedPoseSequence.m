function [ output_args ] = loadRecordedPoseSequence( filename, action_name )
%LOADRECORDEDPOSESEQUENCE Summary of this function goes here
%   Detailed explanation goes here
% load the recorded poses from file

output_args.actionData = [];
one_action_data = struct('action', action_name, 'marg_ind', [], 'pair_ind', []);
one_pose = [];
output_args.poseData = zeros(1, 10, 3);

pose_index = 0; % keep track of the amount of pose we have encountered
    % this goes into the tmp_action_index

fid = fopen(filename); % open the file
tline = fgetl(fid); % read one line
while ischar(tline) % if we haven't reach eof
    if strcmp(tline, 'end_action')
        % if we reach to a end, means one action is completed, then
        % we need to push the action indexes to the actionData
        one_action_data.pair_ind = one_action_data.marg_ind(1, 1:end-1);
        output_args.actionData = [output_args.actionData one_action_data];
        one_action_data.marg_ind = [];
        one_action_data.pair_ind = [];
    elseif strcmp(tline, 'end_pose')
        pose_index = pose_index + 1;
        output_args.poseData(pose_index, :, :) = one_pose;
        one_action_data.marg_ind = [ one_action_data.marg_ind, pose_index ];
        one_pose = [];
    else
        pieces = strsplit(tline, ','); % split the string using comma
        one_joint = [];
        for step = 1:3 % from 1 to 3 is the y,x,angle
            one_joint = [one_joint, str2num(pieces{1,step})]; %populate one sample
        end
        one_pose = [one_pose; one_joint]; % add this sample to the data set
    end
    tline = fgetl(fid); % read another line
end
fclose(fid);

end

