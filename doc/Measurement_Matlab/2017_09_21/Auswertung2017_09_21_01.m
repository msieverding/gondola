%% Second 2D measurements
% better than day before

clearvars;
%% Import the data
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_09_21\2017_09_21.xls','01');
raw = raw(3:end,1:5);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
Punkt = data(:,1);
X = data(:,2);
Y = data(:,3);
MeasX = data(:,4);
MeasY = data(:,5);
OffsetX = 2364;
OffsetY = 1200;
Anchor1X = 0;
Anchor1Y = 0;
Anchor2X = 0;
Anchor2Y = 3236;
CalibrationPointX = 400;
CalibrationPointY = 1500;

%% Clear temporary variables
clearvars data raw;

%% Close all figures
close all;

%% Computations
DistX = - MeasX(:) + OffsetX;
DistY = MeasY(:) - OffsetY;
ErrX = DistX(:) - X(:);
ErrY = DistY(:) - Y(:);

%% Heatmap
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.2, 0.6, 0.4]); 
hold on;
title("Accuracy in 2D with start at (x=400/y=1500)");
set(gca,'YDir','reverse');      % reverse direction of y axis
plot(Y, X, 'b.');
plot(DistY, DistX, 'r.');
h = plot(CalibrationPointY, CalibrationPointX, 'go');
set(h, 'linewidth', 1.5);
h = plot([Anchor1Y Anchor2Y], [Anchor1X Anchor2X], 'ko');
set(h, 'linewidth', 1.5);
legend("Input", "Output", "Calibration Point", "Motor", 'Location' ,'best');
xlim([0 3236]);
ylim([0 2364]);
xlabel("Y Direction [mm]");
ylabel("X Direction [mm]");

%% Spooling Errors
SpoolMotor1 = sqrt((Anchor1X - X(:)).^2 + (Anchor1Y - Y(:)).^2);
[SpoolMotor1, sortIndexMotor1] = sort(SpoolMotor1);
SpoolDistMotor1 = sqrt((Anchor1X - DistX(:)).^2 + (Anchor1Y - DistY(:)).^2);
SpoolDistMotor1 = SpoolDistMotor1(sortIndexMotor1);
SpoolErrMotor1 = SpoolDistMotor1 - SpoolMotor1;
SpoolCalibrationMotor1 = sqrt((Anchor1X - CalibrationPointX).^2 + (Anchor1Y - CalibrationPointY).^2);

SpoolMotor2 = sqrt((Anchor2X - X(:)).^2 + (Anchor2Y - Y(:)).^2);
[SpoolMotor2, sortIndexMotor2] = sort(SpoolMotor2);
SpoolDistMotor2 = sqrt((Anchor2X - DistX(:)).^2 + (Anchor2Y - DistY(:)).^2);
SpoolDistMotor2 = SpoolDistMotor2(sortIndexMotor2);
SpoolErrMotor2 = SpoolDistMotor2 - SpoolMotor2;
SpoolCalibrationMotor2 = sqrt((Anchor2X - CalibrationPointX).^2 + (Anchor2Y - CalibrationPointY).^2);

%% Spooled Distance Motor1
figure;
hold on;
plot(SpoolMotor1, 'b.');
plot(SpoolDistMotor1, 'r.');
line([0 length(SpoolMotor1)], [SpoolCalibrationMotor1 SpoolCalibrationMotor1], 'Color', 'g');
title("Spooled distance of Motor 1");
xlabel("Measurement Number");
ylabel("Spooled distance [mm]");
legend("Input", "Output", "Calibrated length", 'Location' ,'best');

%% Spooled Distance Motor2
figure;
hold on;
plot(SpoolMotor2, 'b.');
plot(SpoolDistMotor2, 'r.');
line([0 length(SpoolMotor2)], [SpoolCalibrationMotor2 SpoolCalibrationMotor2], 'Color', 'g');
title("Spooled distance of Motor 2");
xlabel("Measurement Number");
ylabel("Spooled distance [mm]");
legend("Input", "Output", "Calibrated length", 'Location' ,'best');

%% Spool Error Motors
f = figure;
ylim([-40 40]);
set(f, 'Units', 'normalized', 'Position', [0.2, 0.2, 0.6, 0.4]); 
hold on;
h = plot(SpoolMotor1 - SpoolCalibrationMotor1, SpoolErrMotor1, 'rx');
set(h, 'linewidth', 1.5);
h = plot(SpoolMotor2 - SpoolCalibrationMotor2, SpoolErrMotor2, 'mx');
set(h, 'linewidth', 1.5);
grid on;
title("Spooling error");
xlabel("Spooled distance from calibration point [mm]");
ylabel("Error [mm]");
legend("Motor 1", "Motor 2", 'Location' ,'best');
