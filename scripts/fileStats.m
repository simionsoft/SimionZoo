function txt=fileStats(file1)

    columns=[7 9 11 12 16 17 18 19];
    absvalue=[1 1 1 1 0 0 0 0];

   
     f=fopen(file1,'r');
     line=fgets(f);
     fclose(f);
     names= strread(line,'%s','delimiter','/');

    txt= sprintf ('%s\n',file1);

    data= dlmread(file1,' ',2,0);

    for i=1:size(columns,2)
        if absvalue(i)>0
            m=mean(abs(data(:,columns(i)+2)));
            s=std(abs(data(:,columns(i)+2)));
        else
            m=mean((data(:,columns(i)+2)));
            s=std((data(:,columns(i)+2)));
        end
        mi=min(data(:,columns(i)+2));
        ma=max(data(:,columns(i)+2));
        line= sprintf( '\n%s:: mean= %f  st.d.= %f [%f,%f]\n',char(names(columns(i)+2)),m,s,mi,ma);
        txt= strcat(txt,line);
    end
   
end