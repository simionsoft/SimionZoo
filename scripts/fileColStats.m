function [m,s,mi,ma]=fileColStats(file1,column)

   
    %f=fopen(file1,'r');
    %line=fgets(f);
    %fclose(f);
    %names= strread(line,'%s','delimiter','/');


    data1= dlmread(file1,' ',2,0);

   % numcols=size(data1,2)-1;
   m=mean(data1(:,column+2));
   s=std(data1(:,column+2));
   mi=min(data1(:,column+2));
   ma=max(data1(:,column+2));
end