function createMexPortProject(funName)
%
% creates a new mex Xcode or MSVS project from a template 
%
% Olaf Schleusing
% (c) 2012
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License along
% with this program; if not, write to the Free Software Foundation, Inc.,
% 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


try
    
    if ismac 
        pathBegin = filesep;
    else
        pathBegin = '';
    end
        
    p = mfilename('fullpath');
    p = regexp(fileparts(p), filesep, 'split');
    toolDir = fullfile(pathBegin, p{:}, filesep);
    allProjectsDir = fullfile(pathBegin, p{1:end-1}, 'mexPort_projects', filesep);
    clear p;

    if nargin < 1
        funName = input('Choose a name for your Mex function: ', 's');
    end
    

    projDir = [fullfile(allProjectsDir, funName) filesep];
    fprintf('\nCreating new MeXPort project in %s\n\n', projDir);
    
    assertDirsExist(allProjectsDir, toolDir);

    if (exist(projDir, 'dir'))
        disp('A project with this name already exists. Aborting.');
        return;
    end
    
    fprintf('\nCreating project %s\n', projDir);


    templateProjName = 'mexPortTemplate';
    oldFunName = templateProjName;

    mkdir(projDir);    
    templateDir = fullfile(toolDir, 'templateFiles', filesep);

    substituteAndCopyFile(fullfile(templateDir, 'cppEntry.cpp'), fullfile(projDir, 'cppEntry.cpp'), ...
        {{oldFunName, funName}});
    substituteAndCopyFile(fullfile(templateDir, 'cppEntry.h'), fullfile(projDir, 'cppEntry.h'), ...
        {{oldFunName, funName}});

    if ismac
        copyfile(fullfile(templateDir, [templateProjName '.xcodeproj']), ...
            fullfile(projDir, [templateProjName '.xcodeproj']));
        system(sprintf('open %s/%s.xcodeproj', projDir, templateProjName));
        fprintf('\nOpening Xcode project.\n')
    else
        copyfile(fullfile(templateDir, 'MSVC', [templateProjName '.vcxproj']), ...
            fullfile(projDir, [funName '.vcxproj']));
        copyfile(fullfile(templateDir, 'MSVC', [templateProjName '.vcxproj.user']), ...
            fullfile(projDir, [funName '.vcxproj.user']));
        copyfile(fullfile(templateDir, 'MSVC', [templateProjName '.vcxproj.filters']), ...
            fullfile(projDir, [funName '.vcxproj.filters']));
       writeMexDefinitionFile(projDir, funName); 
       
       system(sprintf('explorer %s', allProjectsDir));
    end
    
    fprintf('\nDone.\n');
    
catch excThrown
        fprintf('ERROR: %s\n\n', excThrown.message);
        if exist([projDir filesep], 'dir')
            sprintf('%s partially created, needs manual cleanup', projDir);
        end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function substituteAndCopyFile(src, dst, replPairs)

srcId = fopen(src, 'rt');
dstId = fopen(dst, 'wt');

while 1
    l = fgetl(srcId);
    if -1 == l
        break;
    end
    for idx = 1:length(replPairs)
        l = strrep(l, replPairs{idx}{1}, replPairs{idx}{2});
    end
    fprintf(dstId, '%s\n', l);
end

fclose(srcId);
fclose(dstId);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function assertDirsExist(allProjectsDir, toolDir)

if ~exist(allProjectsDir, 'dir')
    mkdir(allProjectsDir);
end
if ~ismac
    if ~exist(fullfile(allProjectsDir, 'AllProjects.sln'), 'file')
        copyfile(fullfile(toolDir, 'templateFiles', 'AllProjects.sln'), ...
            fullfile(allProjectsDir, 'AllProjects.sln'));
    end
end
if ~exist(fullfile(allProjectsDir, 'utils'), 'dir')
    mkdir(fullfile(allProjectsDir, 'utils'));
    copyfile(fullfile(toolDir, 'utils', 'mexPort_template.h'), ...
        fullfile(allProjectsDir, 'utils', 'mexPort.h'));
    copyfile(fullfile(toolDir, 'utils', 'utils_template.h'), ...
        fullfile(allProjectsDir, 'utils', 'utils.h'));
    copyfile(fullfile(toolDir, 'utils', 'utils_template.cpp'), ...
        fullfile(allProjectsDir, 'utils', 'utils.cpp'));
end
if ~exist(fullfile(allProjectsDir, 'Matlab'), 'dir')
    mkdir(fullfile(allProjectsDir, 'Matlab'));
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function writeMexDefinitionFile(p, funName)
    fid = fopen(fullfile(p, 'mexFunction.def'), 'wt');
    fwrite(fid, sprintf(['LIBRARY "' funName '\n']));
    fwrite(fid, sprintf('EXPORTS\n'));
    fwrite(fid, sprintf('\tmexFunction\n'));
    fclose(fid);
