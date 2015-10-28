f= fopen('stats-v3.txt','w');

%BOUKHEZZAR
stats= fileStats('../logs/boukhezzar/boukhezzar-log-0-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/b-learn-1/ac-boukhezzar-log-0-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/b-learn-1/ac-boukhezzar-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/b-learn-2/ac-boukhezzar-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/b-learn-3/ac-boukhezzar-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-constant/b-learn-1/ac-boukhezzar-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-constant/b-learn-2/ac-boukhezzar-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-constant/b-learn-3/ac-boukhezzar-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);

% 
% %VIDAL
stats= fileStats('../logs/vidal/vidal-log-0-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/v-learn-1/ac-vidal-log-0-1.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/v-learn-1/ac-vidal-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/v-learn-2/ac-vidal-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-dec/v-learn-3/ac-vidal-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-constant/v-learn-1/ac-vidal-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-constant/v-learn-2/ac-vidal-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);
stats= fileStats('../logs/ii-3-constant/v-learn-3/ac-vidal-log-0-981.txt');
fprintf(f,'%s\n\n\n',stats);

fclose(f);