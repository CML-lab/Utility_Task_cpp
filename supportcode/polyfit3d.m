%fit a polynomial surface up to order 2 to a set of data
function [coef] = polyfit3d(x,y,z,deg,varargin)

%function modified from https://www.ram.ewi.utwente.nl/aigaion/attachments/single/190
% Interpolation of 3D Surfaces for Contact Modeling, by Frank Ambrosius
% March 2005 



i = 1;
fig = get(groot,'CurrentFigure');
if ~isempty(fig)
    fig = fig.Number+1;
else
    fig = 1;
end
xlab = 'x';
ylab = 'y';
zlab = 'z';


while i < length(varargin)
    switch(varargin{i})
        case 'fig'
            fig = varargin{i+1};
            i = i+2;
        case 'xlab'
            xlab = varargin{i+1};
            i = i+2;
        case 'ylab'
            ylab = varargin{i+1};
            i = i+2;
        case 'zlab'
            zlab = varargin{i+1};
            i = i+2;            
        otherwise
            i = i+1;
    end
end

    
nx = length(x); 
test = 0;


%plot the outcome
figure(fig);
h = plot3(x,y,z,'k.');
set(h,'MarkerSize',20);
hold on

xlims = get(gca,'xlim');
ylims = get(gca,'ylim');
xplt = linspace(xlims(1),xlims(2),50);
yplt = linspace(ylims(1),ylims(2),50);
[xplt,yplt] = meshgrid(xplt,yplt);


if deg == 1
    %1st degree polynomial surface
    
    % Create Vandermonde matrix
    V = ones(length(x),1);
    V(:,2) = x;
    V(:,3) = y;
    
    A = (V'* V) \ V' * z; % Calculate the coeficients
    coef = A;
    
    zp = 1;
    for a = 1:length(xplt)
        for b = 1:length(yplt)
            zp(a,b,1) = 1;
            zp(a,b,2) = xplt(a,b);
            zp(a,b,3) = yplt(a,b);
        end
    end
    
    
elseif deg == 2
    %2nd degree polynomial surface
    
    % Create Vandermonde matrix
    V = ones(length(x),1);
    V(:,2) = x;
    V(:,3) = x.^2;
    V(:,4) = x.*y;
    V(:,5) = y;
    V(:,6) = y.^2;
    
    
    A = (V'* V) \ V' * z; % Calculate the coeficients
    coef = A;
    
    zp = 1;
    for a = 1:length(xplt)
        for b = 1:length(yplt)
            zp(a,b,1) = 1;
            zp(a,b,2) = xplt(a,b);
            zp(a,b,3) = xplt(a,b).^2;
            zp(a,b,4) = xplt(a,b).*yplt(a,b);
            zp(a,b,5) = yplt(a,b);
            zp(a,b,6) = yplt(a,b).^2;
        end
    end
   
else
    disp('Polynomial Degree currently undefined')
    coef = [];
    
end

for i = 1:size(zp,1)
    for j = 1:size(zp,2)
        zplt(i,j) = dot(squeeze(zp(i,j,:)),A);
    end
end

surf(xplt,yplt,zplt);
%surf(xplt,yplt,0.5*ones(size(zplt)));
surf(get(gca,'xlim'),get(gca,'ylim'),0.5*ones(2,2));

hold off;
xlabel('Sure bet')
ylabel('Gamble expected value');
zlabel('p[Gamble]');
grid on;

