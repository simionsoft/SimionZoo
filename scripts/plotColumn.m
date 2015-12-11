function plotColumn(file,column,color,linestyle,firstLine,minx,maxx)

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
    
    s=size(data,1);
    x1=cast(s*minx,'int32');
    x1= max(1,x1);
    x2=cast(s*maxx,'int32');
    x2= min(size(data,1),x2);
    
    switch rem(linestyle,5)
        case 0
            plot(data(x1:x2,1),data(x1:x2,column),'-','color',colorname);
        case 1
            plot(data(x1:x2,1),data(x1:x2,column),'--','color',colorname);
        case 2
            plot(data(x1:x2,1),data(x1:x2,column),'.-','color',colorname);
        case 3
            plot(data(x1:x2,1),data(x1:x2,column),':','color',colorname);
        case 4
            plot(data(x1:x2,1),data(x1:x2,column),'-.','color',colorname);
    end


        
  %  plot(downsample(data(1:(maxy-miny+1),1),downfactor),downsample(data(miny:maxy,column),downfactor),linestyle,'color',colorname);
 
 
    xlim([data(x1,1) data(x2,1)]);% data((x2-x1+1),1)]);
end