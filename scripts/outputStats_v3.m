f= fopen('../data/stats-v3.txt','w');

%VIDAL
stats= fileStats('../logs/wind-turbine/wt-test-vidal-evaluation-1-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/wind-turbine/wt-learn-vidal-evaluation-1-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/wind-turbine/wt-learn-vidal-evaluation-1-1000.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/wind-turbine/wt-learn-vidal-2-evaluation-1-1000.txt');
fprintf(f,'%s\n\n\n',stats);

%BOUKHEZZAR
stats= fileStats('../logs/wind-turbine/wt-test-boukhezzar-evaluation-1-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/wind-turbine/wt-learn-boukhezzar-evaluation-1-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/wind-turbine/wt-learn-boukhezzar-evaluation-1-1000.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/wind-turbine/wt-learn-boukhezzar-2-evaluation-1-1000.txt');
fprintf(f,'%s\n\n\n',stats);

fclose(f);