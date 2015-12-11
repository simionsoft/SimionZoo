function plot_wind_profile()
h=figure();

set(0,'defaultLineLineWidth',0.5);   % set the default line width to lw
set(0,'defaultLineMarkerSize',0.5); % set the default line marker size to msz






minx=0.0;
maxx=1.0;
dsfactor=20;


i=5;
hold off;
plotColumn('../logs/wind-turbine/wt-test-vidal-evaluation-1-1.txt',i,2,0,2,minx,maxx);


    %AXIS LABELS
    xl= xlabel('$Time (s)$','FontSize',14);
    set(xl,'Interpreter','Latex');
    yl= ylabel('Wind speed $v$ (m/s)','FontSize',14);
    set(yl,'Interpreter','Latex');
    
filename= sprintf('../images/wind-profile-v2');
saveas(h,filename,'fig');
print(h,'-depsc', filename);

