function plotExperiments(file1,file2,downfactor)

    h= figure();
    f=fopen(strcat('../logs/' , file1),'r');
    line=fgets(f);
    fclose(f);
    names= strread(line,'%s','delimiter','/');


    data1= dlmread(strcat('../logs/' , file1),' ',2,0);
    data2= dlmread(strcat('../logs/' , file2),' ',2,0);
    numcols=size(data1,2)-1;
   for i=2:numcols
       clf();
       hold on;
       plot(downsample(data1(:,1),downfactor),downsample(data1(:,i),downfactor),'color','blue');
       plot(downsample(data2(:,1),downfactor),downsample(data2(:,i),downfactor),'color','red');
       filename= sprintf('../images/%s', char(names(i)));
       print('-dpng','-r0', filename);
%       !hgexport(h,filename);
%       class(filename);
   end
end