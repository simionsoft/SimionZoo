function plotColumn(file,column,color,linestyle,firstLine)
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
    
    data= dlmread(file,' ',firstLine,0);
    
    
    switch rem(linestyle,4)
        case 0
            plot(data(:,1),data(:,column),'-','color',colorname);
        case 1
            plot(data(:,1),data(:,column),'--','color',colorname);
        case 2
            plot(data(:,1),data(:,column),'.-','color',colorname);
        case 3
            plot(data(:,1),data(:,column),':','color',colorname);
    end


        
  %  plot(downsample(data(1:(maxy-miny+1),1),downfactor),downsample(data(miny:maxy,column),downfactor),linestyle,'color',colorname);
 
  
  %  xlim([0 data((maxy-miny+1),1)]);
end