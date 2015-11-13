function plotColumn(file,column,color,linestyle)
%function plotColumn(file,column,downfactor,color,miny,maxy,linestyle)
    switch color
        case 1
            colorname='red';
        case 2
            colorname='blue';
        case 3
            colorname='green';
        case 4
            colorname='magenta';
        case 5
            colorname='yellow';
        otherwise
            colorname='magenta';
    end
    %data= dlmread(file,' ',2,0);
    data= dlmread(file,' ',0,0);
        
  %  plot(downsample(data(1:(maxy-miny+1),1),downfactor),downsample(data(miny:maxy,column),downfactor),linestyle,'color',colorname);
  plot(data(:,1),data(:,column),linestyle,'color',colorname);
  
  %  xlim([0 data((maxy-miny+1),1)]);
end