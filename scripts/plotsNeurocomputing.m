function plotsNeurocomputing()
h=figure();


 set(0,'defaultLineLineWidth',1.5);   % set the default line width to lw
 set(0,'defaultLineMarkerSize',1.5); % set the default line marker size to msz
 
 % CONDITIONING COMPARISON
 dir={'wind-turbine';'pitch';'underwater-vehicle'};
 prefix={'wt-learn-vidal';'pitch-learn';'uv-learn'};
 prefix1={''; '-2'; '-3'; '-4'};
 critics={'td';'totd'};
 cond= {'no-conditioning';'conditioned-actor';'conditioned-actor-critic'; 'trained'};

 for d=1:size(dir,1)
     for cr=1:size(critics,1)
         for i=1:size(prefix1,1)
            %PLOTS
            hold off;
            for c= 1:size(cond,1)
                inputFilename= strcat('../logs/',dir{d},'/',cond{c},'/',prefix{d},'-',critics{cr},prefix1{i},'-summary-1.txt');
                plotColumn(inputFilename,2,c,c,0);
                hold on;
            end
        
        filename= strcat('../images/',dir{d},'-conditioning-comparison-',critics{cr},prefix1{i});

        %LEGEND
        l= legend('$A-C$','$A^{i}-C$','$A^{i}-C^{i}$','$A^{t}-C^{t}$');
        set(l,'Interpreter','Latex','Location','southeast');
        %AXIS LABELS
        xl= xlabel('$Time (s)$','FontSize',14);
        set(xl,'Interpreter','Latex');
        
        lab= '$Average\:rewards$';
        yl= ylabel(lab,'FontSize',14);
        set(yl,'Interpreter','Latex');

        %SIZE MATTERS
        set(gca,'units','inches','position', [0.6 0.5 5.2 2.4]);
        set(h, 'units', 'inches', 'position', [0 0 6 3],'PaperPosition',[0 0 4 1]);
        set(gcf, 'PaperPositionMode','auto');

        %SAVE

        saveas(h,filename,'fig');
        print(h,'-depsc', filename);
         end
    end
 end
 
 %%NUMBER OF FEATURES COMPARISON
 
 dir={'wind-turbine';'pitch';'underwater-vehicle'};
 prefix={'wt-learn-vidal';'pitch-learn';'uv-learn'};
 prefix1={''; '-2'; '-3'; '-4'};
 critics={'td';'totd'};
 cond= {'no-conditioning';'conditioned-actor';'conditioned-actor-critic';'trained'};

 for d=1:size(dir,1)
    for cr=1:size(critics,1)
     for c=1:size(cond,1)
         
            %PLOTS
         hold off;
         for i=1:size(prefix1,1)
            inputFilename= strcat('../logs/',dir{d},'/',cond{c},'/',prefix{d},'-',critics{cr},prefix1{i},'-summary-1.txt');
                
            plotColumn(inputFilename,2,i,i,0);
            hold on;
         end
        
        filename= strcat('../images/',dir{d},'-numfeatures-comparison-',cond{c},'-',critics{cr});

        %LEGEND
        l= legend('$f^{1}$','$f^{2}$','$f^{3}$','$f^{4}$');
        set(l,'Interpreter','Latex','Location','southeast');
        %AXIS LABELS
        xl= xlabel('$Time (s)$','FontSize',14);
        set(xl,'Interpreter','Latex');
        
        lab= '$Average\:rewards$';
        yl= ylabel(lab,'FontSize',14);
        set(yl,'Interpreter','Latex');

        %SIZE MATTERS
        set(gca,'units','inches','position', [0.6 0.5 5.2 2.4]);
        set(h, 'units', 'inches', 'position', [0 0 6 3],'PaperPosition',[0 0 4 1]);
        set(gcf, 'PaperPositionMode','auto');

        %SAVE

        saveas(h,filename,'fig');
        print(h,'-depsc', filename);
     end
    end
 end
 
 
%% CRITIC COMPARISON 

 dir={'wind-turbine';'pitch';'underwater-vehicle'};
 prefix={'wt-learn-vidal';'pitch-learn';'uv-learn'};
 prefix1={''; '-2'; '-3'; '-4'};
 cond= {'no-conditioning';'conditioned-actor';'conditioned-actor-critic';'trained'};

 for d=1:size(dir,1)
     for i=1:size(prefix1,1)
         for c=1:size(cond,1)
        %PLOTS
        hold off;
       % set(0,'defaultLineLineWidth',1);         
        plotColumn(strcat('../logs/',dir{d},'/',cond{c},'/',prefix{d},'-td',prefix1{i},'-summary-1.txt'),2,1,1,0);
        hold on;
      %  set(0,'defaultLineLineWidth',1.5);
        plotColumn(strcat('../logs/',dir{d},'/',cond{c},'/',prefix{d},'-totd',prefix1{i},'-summary-1.txt'),2,2,2,0);
        filename= strcat('../images/',dir{d},'-critic-comparison-',prefix1{i},cond{c});

        %LEGEND
        l= legend('$TD(\lambda)$','$TOTD(\lambda)$');
        set(l,'Interpreter','Latex','Location','southeast');
        %AXIS LABELS
        xl= xlabel('$Time (s)$','FontSize',14);
        set(xl,'Interpreter','Latex');

        lab= '$Average\:rewards$';
        yl= ylabel(lab,'FontSize',14);
        set(yl,'Interpreter','Latex');

        %SIZE MATTERS
        set(gca,'units','inches','position', [0.6 0.5 5.2 2.4]);
        set(h, 'units', 'inches', 'position', [0 0 6 3],'PaperPosition',[0 0 4 1]);
        set(gcf, 'PaperPositionMode','auto');

        %SAVE

        saveas(h,filename,'fig');
        print(h,'-depsc', filename);
         end
     end
 end
