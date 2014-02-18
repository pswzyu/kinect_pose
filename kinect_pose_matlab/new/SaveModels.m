% save the models for actions into file to be read from c++

function re = SaveModels( models, filename )

NM = length(models); % number of models, number of action types
NC = length( models(1).P.c ); % number of pose classes, the clusters
NP = length( models(1).P.clg ); % number of body parts, always be 10

% write meta data
dlmwrite( filename, [NM, NP, NC], '-append' );

for step = 1:NM
    % write the start sign
    dlmwrite( filename, '=', '-append' );
    % write the P.c
    dlmwrite( filename, models(step).P.c, '-append' );
    % write the P.clg
    for step2 = 1:NP
        if isempty( models(step).P.clg(step2).mu_y )
            dlmwrite( filename, 'n', '-append' );
        else
            dlmwrite( filename, models(step).P.clg(step2).mu_y, '-append' );
        end
        dlmwrite( filename, models(step).P.clg(step2).sigma_y, '-append' );
        if isempty( models(step).P.clg(step2).mu_x )
            dlmwrite( filename, 'n', '-append' );
        else
            dlmwrite( filename, models(step).P.clg(step2).mu_x, '-append' );
        end
        dlmwrite( filename, models(step).P.clg(step2).sigma_x, '-append' );
        if isempty( models(step).P.clg(step2).mu_angle )
            dlmwrite( filename, 'n', '-append' );
        else
            dlmwrite( filename, models(step).P.clg(step2).mu_angle, '-append' );
        end
        dlmwrite( filename, models(step).P.clg(step2).sigma_angle, '-append' );
        if isempty( models(step).P.clg(step2).theta )
            dlmwrite( filename, 'n', '-append' );
        else
            dlmwrite( filename, models(step).P.clg(step2).theta, '-append' );
        end
    end
    % write transmatrix
    dlmwrite( filename, models(step).P.transMatrix, '-append' );
end

fclose('all');
end