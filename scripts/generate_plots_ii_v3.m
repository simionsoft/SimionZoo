function generate_plots_ii()
h=figure();


 set(0,'defaultLineLineWidth',1.5);   % set the default line width to lw
 set(0,'defaultLineMarkerSize',1.5); % set the default line marker size to msz




f=fopen('../logs/vidal/vidal-log-0-1.txt','r');
line=fgets(f);
fclose(f);
names= strread(line,'%s','delimiter','/');

minx=1000;
maxx=3000;
dsfactor=10;

% #define DIM_P_set 0 //P_setpoint
% #define DIM_P_e 1
% #define DIM_P_a 2 //P_a
% #define DIM_v 3 //v_w
% #define DIM_T_a 4 //T_a
% #define DIM_omega_r 5 //omega_r
% #define DIM_d_omega_r 6 //d_omega_r
% #define DIM_beta 7 //beta
% #define DIM_d_beta 8 //d_beta
% #define DIM_T_g 9  //T_g
% #define DIM_d_T_g 10//d_T_g
% #define DIM_P_error 11
% #define DIM_omega_r_error 12
% #define DIM_integrative_omega_r_error 13


numcols=size(names,1)-1;
for i=[7 8 9 11 13 14] %column + 2 (time + 1-indexed)
%VIDAL CONTROLLER%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %PLOTS
    hold off;
   % set(0,'defaultLineLineWidth',1);
    plotColumn('../logs/vidal/vidal-log-0-1.txt',i,dsfactor,1,minx,maxx,'-');
    hold on;
  %  set(0,'defaultLineLineWidth',1.5);
    plotColumn('../logs/ii-3-dec/v-learn-1/ac-vidal-log-0-1.txt',i,dsfactor,2,minx,maxx,'--');
    plotColumn('../logs/ii-3-dec/v-learn-1/ac-vidal-log-0-981.txt',i,dsfactor,3,minx,maxx,'-.');
    plotColumn('../logs/ii-3-constant/v-learn-1/ac-vidal-log-0-981.txt',i,dsfactor,4,minx,maxx,'-.'); %-.

    %LEGEND
    l= legend('$\pi_v$','${\hat{\pi}}_v$','$\hat{\pi}_{v}^{*}$','$\hat{\pi}_{v}^{**}$');
    set(l,'Interpreter','Latex');
    %AXIS LABELS
    xl= xlabel('$Time (s)$','FontSize',14);
    set(xl,'Interpreter','Latex');
    switch i
        case 7
            lab= '$\omega_r$';
        case 8
            lab= '$\dot{\omega_r$';
        case 9
            lab= '$\beta\:(rad)$';
        case 11
            lab='$T_g\:(Nm)$';
        case 13
            lab='$e_p\:(W)$';
        case 14
            lab= '$e_{\omega_r}\:(rad/s)$';


        otherwise
            lab= 'null';
    end
    yl= ylabel(lab,'FontSize',14);
    set(yl,'Interpreter','Latex');

    %SIZE MATTERS
    set(gca,'units','inches','position', [0.6 0.5 11.2 2.4]);
    set(h, 'units', 'inches', 'position', [0 0 12 3],'PaperPosition',[0 0 4 1]);
    set(gcf, 'PaperPositionMode','auto');

    %SAVE
    filename= sprintf('../images/vidal-%s', char(names(i)));
    saveas(h,filename,'fig');
    print(h,'-depsc', filename);
    
    
%BOUKHEZZAR CONTROLLER%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %PLOTS
    hold off;
 %   set(0,'defaultLineLineWidth',1);
    plotColumn('../logs/boukhezzar/boukhezzar-log-0-1.txt',i,dsfactor,1,minx,maxx,'-');
    hold on;
%    set(0,'defaultLineLineWidth',1.5);
    plotColumn('../logs/ii-3-dec/b-learn-1/ac-boukhezzar-log-0-1.txt',i,dsfactor,2,minx,maxx,'--');
    plotColumn('../logs/ii-3-dec/b-learn-1/ac-boukhezzar-log-0-981.txt',i,dsfactor,3,minx,maxx,'-.');
    plotColumn('../logs/ii-3-constant/b-learn-1/ac-boukhezzar-log-0-981.txt',i,dsfactor,4,minx,maxx,'-.'); %-.

    %LEGEND
    l= legend('$\pi_b$','${\hat{\pi}}_b$','$\hat{\pi}_{b}^{*}$','$\hat{\pi}_{b}^{**}$');
    set(l,'Interpreter','Latex');
%AXIS LABELS
    xl= xlabel('$Time (s)$','FontSize',14);
    set(xl,'Interpreter','Latex');
    switch i
        case 7
            lab= '$\omega_r$';
        case 8
            lab= '$\dot{\omega_r$';
        case 9
            lab= '$\beta\:(rad)$';
        case 11
            lab='$T_g\:(Nm)$';
        case 13
            lab='$e_p\:(W)$';
        case 14
            lab= '$e_{\omega}\:(rad/s)$';


        otherwise
            lab= 'null';
    end
    yl= ylabel(lab,'FontSize',14);
    set(yl,'Interpreter','Latex');
    
    filename= sprintf('../images/boukhezzar-%s', char(names(i)));
    saveas(h,filename,'fig');
    print(h,'-depsc', filename);
end
