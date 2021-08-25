% quickly analyze data from the utility assay (and also select a subset of
%  trials for reward)

clear all;
close all;

do_fixed_EV = 1; 
%set to 1 to compute utility by fixing the Expected Value and searching for
%  an equivalent Sure-Bet amount that makes p[Gamble] = 0.5
%set to 0 to compute utility by fixing the Sure-Bet amount and searching for
%  an equivalent Expected Value that makes p[Gamble] = 0.5



[data,params] = KRload();
data = data{1};
data = rmfield(data,'Time');


data.ExpVal = (data.Gamble1 + data.Gamble2)/2;  %calculate estimated expected value

surebets = unique(data.SureBet);
gamble1 = unique(data.Gamble1);
gamble2 = unique(data.Gamble2);

betmat = [];

betlandscape = [];

for a = 1:length(surebets)
    isb = find(data.SureBet == surebets(a));
    
    for b = 1:length(gamble1)
        ig1 = find(data.Gamble1 == gamble1(b));
        
        if (~isempty(ig1))
            indsg1 = intersect(isb,ig1);
            
            for c = 1:length(gamble2)
                ig2 = find(data.Gamble2 == gamble2(c));
                
                inds = intersect(indsg1,ig2);
                if ~isempty(inds)
                    betmat(a,b,c).bets = [surebets(a) gamble1(b) gamble2(c)];
                    betmat(a,b,c).inds = inds;
                    betmat(a,b,c).lats = data.Lat(inds);
                    betmat(a,b,c).choice = data.Choice(inds);
                    betmat(a,b,c).expval = (gamble1(b)+gamble2(c))/2;
                    betmat(a,b,c).psurebet = 1 - sum(data.Choice(inds)-1)/length(inds);
                    betlandscape = [betlandscape; betmat(a,b,c).bets(1) betmat(a,b,c).expval betmat(a,b,c).psurebet];
                end
            end
        end
    end
end

%fit linear plane through data
%[nvec,Plane,pt] = affine_fit(betlandscape);
%[X,Y] = meshgrid(linspace(min(surebets), max(surebets), 3),linspace(min(data.ExpVal),max(data.ExpVal),3));
%planefit = fit(betlandscape(:,1:2),1-betlandscape(:,3),'poly11');

%fit curved plane through data
% %surffit = fit(betlandscape(:,1:2),1-betlandscape(:,3),'poly22');

x = reshape(betlandscape(:,1),[],1);
y = reshape(betlandscape(:,2),[],1);
z = reshape(1-betlandscape(:,3),[],1);
polycoef1 = polyfit3d(x,y,z,1,'xlab','Sure bet','ylab','Gamble expected value','zlab','p[Gamble]','fig',3);
polycoef2 = polyfit3d(x,y,z,2,'xlab','Sure bet','ylab','Gamble expected value','zlab','p[Gamble]','fig',2);

% % figure(1)
% % h = plot3(betlandscape(:,1),betlandscape(:,2),1-betlandscape(:,3),'k.');
% % set(h,'MarkerSize',20);
% % hold on
% % %surf(X,Y, -(nvec(1)/nvec(3)*X+nvec(2)/nvec(3)*Y-dot(nvec,pt)/nvec(3)),'facecolor','red','facealpha',0.5);
% % %h1 = plot(planefit);
% % %set(h1(1),'FaceColor','red','FaceAlpha',0.6,'EdgeColor','white','EdgeAlpha',0.3,'LineStyle','none')
% % h2 = plot(surffit);
% % surf(get(gca,'xlim'),get(gca,'ylim'),0.5*ones(2,2));
% % %set(h2(1),'FaceColor','blue','FaceAlpha',0.6,'EdgeColor','white','EdgeAlpha',0.3,'LineStyle','none')
% % hold off;
% % xlabel('Sure bet')
% % ylabel('Gamble expected value');
% % zlabel('p[Gamble]');
% % grid on;


if do_fixed_EV
    
    %estimate the utility of a fixed value
    %asking what the value of the sure bet is when when the gamble expected
    %value is X and the p[Gamble] is 0.5. That is, where does the plane cross
    %0.5 when the expected value of the gamble is X?
    
    %vals = [0 5 10 15 20 ];
    vals = [0:.1:30];
    for i = 1:length(vals)
        x = vals(i);
        %A = polycoef(7) + polycoef(8)*x + polycoef(9)*(x^2);
        %B = polycoef(4) + polycoef(5)*x + polycoef(6)*(x^2);
        %C = polycoef(1) + polycoef(2)*x + polycoef(3)*(x^2) - 0.5;
        %A = polycoef(3) + polycoef(6)*x + polycoef(9)*(x^2);
        %B = polycoef(2) + polycoef(5)*x + polycoef(8)*(x^2);
        %C = polycoef(1) + polycoef(4)*x + polycoef(7)*(x^2) - 0.5;
        
        A = polycoef2(6);
        B = polycoef2(5) + polycoef2(4)*x;
        C = polycoef2(1) + polycoef2(2)*x + polycoef2(3)*(x^2) - 0.5;
        planei = (roots([A B C]));
        if isempty(planei)
            plane2val(i) = NaN;
        else
            planei(planei<0) = [];
            if isempty(planei)
                plane2val(i) = NaN;
            else
                [~,ind] = min(abs(planei));
                plane2val(i) = planei(ind);
            end
        end
    end
    
        
    vals = [0:.1:30];
    for i = 1:length(vals)
        x = vals(i);
        A = polycoef1(3);
        B = polycoef1(1) + polycoef1(2)*x;
        plane1val(i) = (0.5-B)/A;
        
    end
    %z = 1 + y + y^2 + x + x * y + x * y^2 + x^2 + x^2 * y + x^2 * y^2
    %substitute y = utility value we want to compute, and z = 0.5; solve for x.
    
    % % int = [0:5:50];
    % %
    % % %planeest = planefit(int,5);  %estimate the probability of gambling for the plane for all values of sure bets and an expected gamble value of 5
    % % %[~,planei] = min(abs(planeest-0.5));
    % %
    % % for i = 1:length(vals)
    % %     x = vals(i);
    % %     surfest = surffit(x,int);
    % %     %[~,surfi] = min(abs(surfest-0.5));
    % %     surfi = ZeroX(int,surfest-0.5);
    % %     [~,ind] = min(abs(surfi));
    % %     surfval(i) = surfi(ind);
    % %
    % % end
    
    
    figure
    plot(vals,plane2val,'ro-');
    hold on;
    % % plot(vals,surfval,'bs:');
    plot(vals,plane1val,'g^-');
    plot(get(gca,'xlim'),[10 10],'k--');
    hold off;
    xlabel('Value');
    ylabel('Equivalent Utility');
    
    
    %calculate the fixed value equivalent to a utility of 10
    util = [5 10 15 20];
    for a = 1:length(util)
        plane2val(imag(plane2val) ~= 0) = NaN;
        
        itmp = find(isnan(plane2val));
        tmppv = plane2val;
        tmppv(itmp) = [];
        tmpv = vals;
        tmpv(itmp) = [];
        
        ev2 = ZeroX(tmpv,tmppv-util(a));
        equivval2(a) = ev2(1);
        
        plane1val(imag(plane1val) ~= 0) = NaN;
        itmp = find(isnan(plane1val));
        tmppv = plane1val;
        tmppv(itmp) = [];
        tmpv = vals;
        tmpv(itmp) = [];
        ev1 = ZeroX(tmpv,tmppv-util(a));
        equivval1(a) = ev1(1);
        
    end
    
    fprintf('\n\nUtility\t     EquivalentValue\n');
    for a = 1:length(util)
        fprintf('   %d\t   %5.2f\t  %5.2f\n',util(a),equivval1(a),equivval2(a));
    end
    
    
else %do_fixed_SureBet
   
    %estimate the utility of a fixed value
    %asking what the value of the expected value is when when the gamble
    %sure-bet value is X and the p[Gamble] is 0.5. That is, where does the
    %plane cross 0.5 when the sure bet is X?
    
    vals = [5 10 15 20 ];
    for i = 1:length(vals)
        x = vals(i);
        
        A = polycoef2(6);
        B = polycoef2(5) + polycoef2(4)*x;
        C = polycoef2(1) + polycoef2(2)*x + polycoef2(3)*(x^2) - 0.5;
        planei = (roots([A B C]));
        planei(planei<0) = [];
        [~,ind] = min(abs(planei));
        plane2val(i) = planei(ind);
        
        A = polycoef1(3);
        B = polycoef1(1) + polycoef1(2)*x;
        plane1val(i) = (0.5-B)/A;
        
    end
    
    figure
    plot(vals,plane2val,'ro-');
    hold on;
    plot(vals,plane1val,'g^-');
    plot([10 10],get(gca,'ylim'),'k--');
    hold off;
    xlabel('Value');
    ylabel('Equivalent Utility');

    fprintf('\n\nUtility\t     EquivalentValue\n');
    for a = 1:length(vals)
        fprintf('   %d\t   %5.2f\t  %5.2f\n',vals(a),plane2val(a),plane1val(a));
    end
    
    
end



% 
% figure(4)
% %x = betlandscape(:,2);
% %y = betlandscape(:,1).*betlandscape(:,3);
% x = betlandscape(:,1);
% y = betlandscape(:,2).*(1-betlandscape(:,3));
% %x(y == 0) = [];
% %y(y == 0) = [];
% plot(x,y,'bo');
% hold on
% %[b,~,~,~,stats] = regress(y,[x ones(size(x))]);
% %plot(get(gca,'xlim'),get(gca,'xlim')*b(1)+b(2),'k-');
% fitx = get(gca,'xlim');
% fitx = fitx(1):1:fitx(2);
% f = polyfit(x,y,2);
% fity = polyval(f,unique(fitx));
% plot(fitx,fity,'k-')
% %plot(get(gca,'xlim'),[10 10],'b:');
% plot([10 10],get(gca,'ylim'),'b:');
% xlabel('sure bet');
% ylabel('E[expected value]')
% xs = unique(x);
% for a = 1:length(xs)
%     ys(a) = mean(y(find(x == xs(a))));
% end
% plot(xs,ys,'rs');
% 
% hold off;



% figure(3)
% x = betlandscape(:,2);
% y = betlandscape(:,1).*betlandscape(:,3);
% %x = betlandscape(:,1);
% %y = betlandscape(:,2).*(1-betlandscape(:,3));
% %x(y == 0) = [];
% %y(y == 0) = [];
% plot(x,y,'bo');
% hold on
% %[b,~,~,~,stats] = regress(y,[x ones(size(x))]);
% %plot(get(gca,'xlim'),get(gca,'xlim')*b(1)+b(2),'k-');
% fitx = get(gca,'xlim');
% fitx = fitx(1):1:fitx(2);
% f = polyfit(x,y,2);
% fity = polyval(f,unique(fitx));
% plot(fitx,fity,'k-')
% %plot(get(gca,'xlim'),[10 10],'b:');
% %plot([10 10],get(gca,'ylim'),'b:');
% plot([10 10],get(gca,'ylim'),'b:');
% xlabel('expected value');
% ylabel('E[sure bet]')
% xs = unique(x);
% for a = 1:length(xs)
%     ys(a) = mean(y(find(x == xs(a))));
% end
% plot(xs,ys,'rs');
% 
% hold off;


%% random sample for payment

%Nsamp = input('N = ');
Nsamp = 5;

rng('shuffle');

scoresum = 100;

while scoresum > 1.3
    
    trialorder = randperm(max(data.Trial));
    
    trials = trialorder(1:Nsamp);
    
    scoresum = sum(data.Outcome(trials))/100;
end


fprintf('Pay: %.2f\n\n',scoresum);



%%

% function ZC = ZeroX(x,y)
% zci = @(v) find(v(:).*circshift(v(:), [-1 0]) <= 0);                    % Returns Approximate Zero-Crossing Indices Of Argument Vector
% zxidx = zci(y);
% ZC = [];
% for k1 = 1:numel(zxidx)
%     idxrng = max([1 zxidx(k1)-1]):min([zxidx(k1)+1 numel(y)]);
%     xrng = x(idxrng);
%     yrng = y(idxrng);
%     ZC(k1) = interp1( yrng(:), xrng(:), 0, 'linear', 'extrap' );
% end
% if isempty(ZC)
%     ZC = NaN;
% end
% end

