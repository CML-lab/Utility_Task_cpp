%generate trial table for Utility assessment
clear all;
close all;

%values in cents
% utilspace = [5*ones(5,1) [10:10:50]' 0*ones(5,1);
%              5*ones(5,1) [10:10:50]' -10*ones(5,1);
%              5*ones(5,1) [10:10:50]' -20*ones(5,1);
%              10*ones(7,1) [10:10:70]' 0*ones(7,1);
%              10*ones(7,1) [10:10:70]' -10*ones(7,1);
%              10*ones(7,1) [10:10:70]' -20*ones(7,1);
%              20*ones(8,1) [20:10:90]' 0*ones(8,1);
%              20*ones(8,1) [20:10:90]' -10*ones(8,1);
%              20*ones(8,1) [20:10:90]' -20*ones(8,1);
%              25*ones(7,1) [30:10:90]' 0*ones(7,1);
%              25*ones(7,1) [30:10:90]' -10*ones(7,1);
%              25*ones(7,1) [30:10:90]' -20*ones(7,1);
%             ];

utilspace = [5*ones(5,1) [10:10:50]' 0*ones(5,1);
             %5*ones(5,1) [10:10:50]' -10*ones(5,1);
             10*ones(5,1) [10 20:20:80]' 0*ones(5,1);
             %10*ones(5,1) [10 20:20:80]' -20*ones(5,1);
             20*ones(5,1) [20:20:100]' 0*ones(5,1);
             %20*ones(5,1) [20:20:100]' -20*ones(5,1);
             25*ones(5,1) [30 40:20:100]' 0*ones(5,1);
             %25*ones(4,1) [40:20:100]' -20*ones(4,1);
            ];

nreps = 5;

rng('shuffle');

gambleorder = round(rand(nreps*size(utilspace,1),1));
tmp1 = gambleorder(2:end-3) == gambleorder(1:end-4);
tmp2 = gambleorder(3:end-2) == gambleorder(1:end-4);
tmp3 = gambleorder(4:end-1) == gambleorder(1:end-4);
tmp4 = gambleorder(5:end) == gambleorder(1:end-4);
tmp = any(tmp1 & tmp2 & tmp3 & tmp4);
while (tmp)
    gambleorder = round(rand(nreps*size(utilspace,1),1));
    tmp1 = gambleorder(2:end-3) == gambleorder(1:end-4);
    tmp2 = gambleorder(3:end-2) == gambleorder(1:end-4);
    tmp3 = gambleorder(4:end-1) == gambleorder(1:end-4);
    tmp4 = gambleorder(5:end) == gambleorder(1:end-4);
    tmp = any(tmp1 & tmp2 & tmp3 & tmp4);
end

gambleorder = reshape(gambleorder,[],nreps);

trials = [];
igamble = 0;
for a = 1:nreps
    reporder = randperm(size(utilspace,1));
    
    for b = 1:length(reporder)
        igamble = igamble+1;
        if gambleorder(igamble) == 1
            trials = [trials; utilspace(reporder(b),[1 3 2])];
        else
            trials = [trials; utilspace(reporder(b),[1 2 3])];
        end
    end
    
end

dlmwrite('utilitytable.txt',trials,'delimiter',' ');
