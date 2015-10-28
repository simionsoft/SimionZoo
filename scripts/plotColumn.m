function plotColumn(file,column,downfactor,color,miny,maxy,linestyle)
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
    data= dlmread(file,' ',2,0);
    %shadedErrorBar(downsample(data(1:(maxy-miny+1),1),downfactor),downsample(data(miny:maxy,column),downfactor),{'r-o','markerfacecolor','r'});
	
        
    plot(downsample(data(1:(maxy-miny+1),1),downfactor),downsample(data(miny:maxy,column),downfactor),linestyle,'color',colorname);
  %  plot(data(1:(maxy-miny)+1),data(miny:maxy,column),'color',colorname);
    xlim([0 data((maxy-miny+1),1)]);%[data(miny,1) data(maxy,1)]);
end