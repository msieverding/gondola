%% First tests in 1D to measure the accuracy
% Measured from the end of the rope to the white palstic.
% Got a first impression of the spooling error

clearvars;
%% Import the data
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_09_19\2017_09_19.xls','01');
raw = raw(3:16,:);

%% Replace non-numeric cells with 0.0
R = cellfun(@(x) (~isnumeric(x) && ~islogical(x)) || isnan(x),raw); % Find non-numeric cells
raw(R) = {0.0}; % Replace non-numeric cells

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
Spooledmm = data(:,1);      % input
Speedcms = data(:,2);       % used speed
Meas1mm = data(:,3);        % measurements
Meas2mm = data(:,4);
Meas3mm = data(:,5);
Meanmm = data(:,6);          % mean of measurements
Measuredmm = [0; data(2:length(data),6) - data(1,6)];   % length of rope
Diffmm = Measuredmm - Spooledmm;                % difference between input and output

%% Clear temporary variables
clearvars data raw R;

f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]); 
hold on;
plot (Spooledmm);
plot (Measuredmm);
legend ("Input", "Output");
title ("Input vs Output");
ylabel ("distance [mm]");
xlabel ("Measurement number");

f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]); 
plot(Spooledmm, Diffmm);
legend("Output - Input");
title ("1D spooling error");
ylabel ("error [mm]");
xlabel ("Spooled distance [mm]");