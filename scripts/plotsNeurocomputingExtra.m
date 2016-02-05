function plotsNeurocomputingExtra()
    h=figure();


    set(0,'defaultLineLineWidth',1.5);   % set the default line width to lw
    set(0,'defaultLineMarkerSize',1.5); % set the default line marker size to msz

    %T_g output
    hold off;
    plotColumn('../logs/wind-turbine/wt-test-vidal-evaluation-1-1.txt',11,1,0,2);
    hold on;
    plotColumn('../logs/wind-turbine/trained/wt-learn-vidal-td-evaluation-1-1.txt',11,2,0,2);

    plotColumn('../logs/wind-turbine/trained/wt-learn-vidal-td-4-evaluation-1-1.txt',11,3,1,2); 

    %LEGEND
    l= legend('$A^t\:-\:n_{fs}^1$','$A^t\:-\:n_{fs}^4$','$\pi_c$');
    set(l,'Interpreter','Latex','Location','southeast');
    %AXIS LABELS^
    xl= xlabel('$Time (s)$','FontSize',14);
    set(xl,'Interpreter','Latex');

    lab= '$T_g\:(Nm)$';
    yl= ylabel(lab,'FontSize',14);
    set(yl,'Interpreter','Latex');

    %SIZE MATTERS
    set(gca,'units','inches','position', [0.6 0.5 5.2 2.4]);
    set(h, 'units', 'inches', 'position', [0 0 6 3],'PaperPosition',[0 0 4 1]);
    set(gcf, 'PaperPositionMode','auto');

    %SAVE

    filename= '../images/extra-compare-target-controller-and-trained-t_g';
    saveas(h,filename,'fig');
    print(h,'-depsc', filename);

        
        
        
    %beta output
    hold off;
    plotColumn('../logs/wind-turbine/wt-test-vidal-evaluation-1-1.txt',9,1,0,2);
    hold on;
    plotColumn('../logs/wind-turbine/trained/wt-learn-vidal-td-evaluation-1-1.txt',9,2,0,2);

    plotColumn('../logs/wind-turbine/trained/wt-learn-vidal-td-4-evaluation-1-1.txt',9,3,1,2); 

    %LEGEND
    l= legend('$A^t\:-\:n_{fs}^1$','$A^t\:-\:n_{fs}^4$','$\pi_c$');
    set(l,'Interpreter','Latex','Location','southeast');
    %AXIS LABELS^
    xl= xlabel('$Time (s)$','FontSize',14);
    set(xl,'Interpreter','Latex');

    lab= '$\beta\:(radians)$';
    yl= ylabel(lab,'FontSize',14);
    set(yl,'Interpreter','Latex');

    %SIZE MATTERS
    set(gca,'units','inches','position', [0.6 0.5 5.2 2.4]);
    set(h, 'units', 'inches', 'position', [0 0 6 3],'PaperPosition',[0 0 4 1]);
    set(gcf, 'PaperPositionMode','auto');

    %SAVE

    filename= '../images/extra-compare-target-controller-and-trained-beta';
    saveas(h,filename,'fig');
    print(h,'-depsc', filename);
 end
 