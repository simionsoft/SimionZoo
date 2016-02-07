function [ output_args ] = plotReward( input_args )
%PLOTREWARD Summary of this function goes here
%   Detailed explanation goes here

error= -1:0.01:1;
error2= -4:0.01:4;
tolerance= 0.1;
tolerance2= 2;
maxr= 10.0;
minr= -10.0;

reward1= 1-(error/tolerance).^2;
%reward1= exp(-(error/tolerance).^2);

reward2= 1-(error2/tolerance2).^2;
%reward2=  exp(-(error2/tolerance2).^2);

reward=zeros(size(reward1,2),size(reward2,2));
for i=1:size(reward1,2)
	for j=1:size(reward2,2);
%          if (reward1(i)<0 && reward2(j)<0)
%          	reward(i,j)=-reward1(i)*reward2(j);
%          elseif reward1(i)<0
%             reward(i,j)=reward1(i);
%          elseif reward2(j)<0
%              reward(i,j)= reward2(j);
%          else
%              reward(i,j)= reward1(i)*reward2(j);
%          end 
        if (abs(error(i))>tolerance)
            reward1(i)= 0.0;
        else
            reward1(i)= 1-(error(i)/tolerance)^2;
        end
        if (abs(error2(j))>tolerance2)
            reward2(j)= 0.0;
        else
            reward2(j)= 1-(error2(j)/tolerance2)^2;
        end
        
        reward(i,j)=(reward1(i)*reward2(j));
     end
end
 
surf(error2,error,reward);

end

