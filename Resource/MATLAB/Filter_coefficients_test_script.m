%% Brief
%{
This script is designed to test if the filter coefficients used for the
DF1 IIR-filter is sufficient for the acoustic filter.

The script creates a signal with a multitude of different frequencies. The
signal is filtered with the coefficients developed for the acoustic-project
for Vortex NTNU.

To indicate the effect of the filter, the frequency-response is plotted
before and after filtering
%}

%% Filter
% Filter-coefficients (numerator)
b0 = 0.32227662;
b1 = 0;
b2 = -0.32227662;
B = [b0, b1, b2];

% Filter coefficients (denominator)
a0 = 1;
a1 = -0.41885608;
a2 = 0.35544676;
A = [a0, a1, a2];

%% Signal generation
% Min frequency [Hz]
f_min = 5000;

% Max frequency [Hz]
f_max = 50000;

% Difference between each signal [Hz]
f_diff = 5000;

% Number of frequencies in the signal
num_f = (f_max + f_min) / f_diff;

% Sampling-frequency
F_s = 150000;

% Length of signal
L = 5000;

% Time (for signal generation)
dt = 1/F_s;
t = (0:L-1)*dt;

% Generating the signal
y = 0;
for f = 0:num_f - 1
    y = y + sin(2*pi*(f_max + f*f_diff)*t);
end

%% FFT and filtering
Y = fft(y);

y_filter = filter(B, A, y);
Y_filter = fft(y_filter);

%% Plotting
%{
The frequency is mirrored at f = 0, where abs(Y(-f_hat)) = abs(Y(f_hat))
  
The plot only consists of the response at f > 0 to make the plot more 
readable 
%}

% Frequency
f = F_s*(0:(L/2))/L;

% The result before filtering
P2_b = abs(Y/L);
P1_b = P2_b(1:L/2+1);
P1_b(2:end-1) = 2*P1_b(2:end-1);

% The result after filtering
P2_a = abs(Y_filter/L);
P1_a = P2_a(1:L/2+1);
P1_a(2:end-1) = 2*P1_a(2:end-1);

subplot(1,2,1);
plot(f, P1_b);
xlabel("f");
ylabel("|Y|");
title("Magnitude before filtering");
grid on;

subplot(1,2,2);
plot(f, P1_a);
xlabel("f");
ylabel("|Y|");
title("Magnitude after filtering");
grid on;

