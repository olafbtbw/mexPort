function makeNewFunctionProject

try
    
    p = mfilename('fullpath');
    solDir = [fileparts(p) filesep];
    clear p;

    funName = input('Choose a name for your function (preferable ending on Cpp): ', 's');
    retValues = input('Specify return value(s) (e.g. [a, b] ): ', 's');
    parameters = input('Specify function parameter(s) (e.g. (d,e,f) ): ', 's');

    if ~isempty(retValues)
        retValues = [retValues ' = '];
    end
    
    funDecl = sprintf('function %s%s%s\n\n', retValues, funName, parameters);
    fprintf('A C++ project for: \n\n');
    fprintf(funDecl);
    fprintf('\nwill be created in %s\n\n', solDir);
    cont = input('Do you wish to continue? (y/n) ', 's');
    if ~strcmpi('Y', cont)
        fprintf('\nAborted\n');
        return
    end

    projName = funName;
    projName(1) = upper(projName(1));
    if exist([solDir projName filesep], 'dir')
        disp('A project with this name already exists. Aborting.');
        return;
    end
    
    fprintf('\nCreating project %s%s\n', solDir, projName);

    makeOutputDirAndMFiles('Debug', funName, retValues, parameters, solDir);
    makeOutputDirAndMFiles('Release', funName, retValues, parameters, solDir);

    templateProjName = 'MyMatlabFunctionNameCpp';
    oldFunName = templateProjName;
    oldFunName(1) = lower(oldFunName(1));
    
    dst = [solDir projName filesep];
    mkdir(dst);
    src = [solDir templateProjName filesep];
    substituteAndCopyFile([src 'PreProcessing.cpp'], [dst 'PreProcessing.cpp'], funName, oldFunName);
    copyfile([src templateProjName '.vcxproj'], [dst projName '.vcxproj'])
    copyfile([src templateProjName '.vcxproj.user'], [dst projName '.vcxproj.user'])
    copyfile([src templateProjName '.vcxproj.filters'], [dst projName '.vcxproj.filters'])


    fid = fopen([solDir projName '\mexFunction.def'], 'wt');
    fwrite(fid, sprintf(['LIBRARY "' funName '\n']));
    fwrite(fid, sprintf('EXPORTS\n'));
    fwrite(fid, sprintf('\tmexFunction\n'));
    fclose(fid);
catch excThrown
        fprintf('ERROR: %s', excThrown.message);
        if ~exist([solDir projName filesep], 'dir')
            sprintf('%s partially created, needs manual cleanup', [solDir projName filesep]);
        end
end

fprintf('\nFinished.\n')

function makeOutputDirAndMFiles(config, funName, retValues, parameters, solDir)

if ~exist([solDir config], 'dir')
    mkdir([solDir config]);
end

fid = fopen([solDir config filesep funName config '.m'], 'wt');
fwrite(fid, sprintf('function %s%s%s%s\n', retValues, funName, config, parameters));
fclose(fid);

function substituteAndCopyFile(src, dst, funName, oldFunName)

srcId = fopen(src, 'rt');
dstId = fopen(dst, 'wt');

while 1
    l = fgetl(srcId);
    if -1 == l
        break;
    end
    l = strrep(l, oldFunName, funName);
    fprintf(dstId, '%s\n', l);
end

fclose(srcId);
fclose(dstId);

