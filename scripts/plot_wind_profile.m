function plot_wind_profile()
h=figure();

set(0,'defaultLineLineWidth',0.5);   % set the default line width to lw
set(0,'defaultLineMarkerSize',0.5); % set the default line marker size to msz






minx=1;
maxx=19999;
dsfactor=20;


i=5;
hold off;
plotColumn('../logs/vidal/vidal-log-0-1.txt',i,dsfactor,1,minx,maxx,'-');


    %AXIS LABELS
    xl= xlabel('$Time (s)$','FontSize',14);
    set(xl,'Interpreter','Latex');
    yl= ylabel('Wind speed (m/s)','FontSize',14);
    set(yl,'Interpreter','Latex');
    
filename= sprintf('../images/wind-profile');
saveas(h,filename,'fig');
print(h,'-depsc', filename);

