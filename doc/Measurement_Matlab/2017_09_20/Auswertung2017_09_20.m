%% First 2D accuracy measurements
% To get an idea of the 2D error
% Measurements were not used later on.

clearvars;
%% Import the data
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_09_20\2017_09_20.xls','2D');
raw = raw(2:end,:);
raw(cellfun(@(x) ~isempty(x) && isnumeric(x) && isnan(x),raw)) = {''};

%% Replace non-numeric cells with NaN
R = cellfun(@(x) ~isnumeric(x) && ~islogical(x),raw); % Find non-numeric cells
raw(R) = {NaN}; % Replace non-numeric cells

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
Punkt = data(:,1);
X = data(:,2);
Y = data(:,3);
MeasX = data(:,4);
MeasY = data(:,5);
DistX = data(:,6);
DistY = data(:,7);
ErrX = data(:,8);
ErrY = data(:,9);
absErr = data(:,10);
% calibration point
anchorPos1X = 0;
anchorPos1Y = 0;
anchorPos2X = 0;
anchorPos2Y = 3236;
calibPointX = 400;
calibPointY = 1500;

%% Clear temporary variables
clearvars data raw R;

%% Heatmap
figure();
hold on;
title("Accuracy in 2D Start (x=400/y=1500)");
set(gca,'YDir','reverse');      % reverse direction of y axis
plot(Y, X, 'b.');
plot(DistY, DistX, 'r.');
plot(calibPointY, calibPointX, 'go');
plot(anchorPos1Y, anchorPos1X, 'ko');
plot(anchorPos2Y, anchorPos2X, 'ko');
legend("Input", "Output", "Calibration Point", "Motor1", "Motor2");
xlim([0 3236]);
ylim([0 2364]);
xlabel("Y Direction [mm]");
ylabel("X Direction [mm]");