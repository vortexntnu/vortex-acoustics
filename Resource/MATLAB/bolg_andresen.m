%got from http://blog.andersen.im/2012/07/signal-emitter-positioning-using-multilateration/


clc;clear;

%================================================
% SETUP
%================================================
v = 1484; %Speed of sound in water
nDim = 3; %Number of dimensions of space
nSensors = 4; %Number of sensors
%only works if nSensors >= 2 + nDim

%Side length of cubic region where emitter may be created
emitterAreaSize = 2000; 
x_star = emitterAreaSize*(rand(nDim,1)-0.5); %Emitter position

disp("emitter position"); disp(x_star)

%Side length of cubic region where sensors may be created
sensorAreaSize = 1000; 
p = sensorAreaSize*(rand(nDim,nSensors)-0.5); %Sensor positions

disp("sensor"); disp(p)

%Time from emitter to sensor
t_stars = sqrt(sum((p-diag(x_star)*ones(nDim, nSensors)).^2,1))/v;
%Sensor closested to emittor
[~,c] = min(t_stars);
%Removing the time for wich the sensors did not know of the signal
t = t_stars-t_stars(c); %The actual timestamps the signals have to work with

%================================================
% COMPUTING POSITION ... see paper
%================================================
ijs = 1:nSensors;
ijs(c) = [];
A = zeros(size(ijs,1), nDim);
b = zeros(size(ijs,1),1);
iRow = 0;
rankA = 0;
for i = ijs
    for j = ijs
        iRow = iRow + 1;
        A(iRow,:) = 2*(v*(t(j)-t(c))*(p(:,i)-p(:,c))'-v*(t(i)-t(c))*(p(:,j)-p(:,c))');
        b(iRow,1) = v*(t(i)-t(c))*(v*v*(t(j)-t(c))^2-p(:,j)'*p(:,j)) ...
            +	(v*(t(i)-t(c))-v*(t(j)-t(c)))*p(:,c)'*p(:,c) ...
            +	v*(t(j)-t(c))*(p(:,i)'*p(:,i)-v*v*(t(i)-t(c))^2);
        rankA = rank(A);
        if(rankA >= nDim)
            break;
        end
    end
    if(rankA >= nDim)
        break;
    end
end

x_star %Actual position of emitter
x_hat_inv = A\b %Calculated position of emitter