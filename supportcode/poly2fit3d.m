%fit a polynomial surface of order 2 to a set of data
function [coef] = poly2fit3d(x,y,z,varargin)

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

nx = length(x); test = 0; %import a surface from stl file
deg = 2; % Degree of the polynom

% Create Vandermonde matrix
p=1;
for i=0:deg
    for j=0:deg
        V(:,p) = (x.^i).*(y.^j);
        p = p + 1;
    end
end
A = (V'* V) \ V' * z; % Calculate the coeficients
coef = A;

%plot the outcome
figure(fig);
h = plot3(x,y,z,'k.');
set(h,'MarkerSize',20);
hold on

xlims = get(gca,'xlim');
ylims = get(gca,'ylim');
xplt = linspace(xlims(1),xlims(2));
yplt = linspace(ylims(1),ylims(2));
[xplt,yplt] = meshgrid(xplt,yplt);

clear z;
p=1;
for i=0:deg
    for j=0:deg
        z(:,:,p) = (xplt.^i).*(yplt.^j);
        p = p + 1;
    end
end
for i = 1:size(z,1)
    for j = 1:size(z,2)
        zplt(i,j) = dot(squeeze(z(i,j,:)),A);
    end
end

surf(xplt,yplt,zplt);

hold off;
xlabel('Sure bet')
ylabel('Gamble expected value');
zlabel('p[Gamble]');
grid on;

