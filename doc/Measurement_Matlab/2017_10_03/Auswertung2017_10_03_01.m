%% Import data from spreadsheet
% Script for importing data from the following spreadsheet:
%
%    Workbook: C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_10_03\2017_10_03.xls
%    Worksheet: 01
%
% To extend the code for use with different selected data or a different
% spreadsheet, generate a function instead of a script.

% Auto-generated by MATLAB on 2017/10/03 21:38:43

clearvars;
close all;
%% Import the data
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_10_03\2017_10_03.xls','01');
raw = raw(69:78,2:15);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
Measurement = data(1,:);
StartX = data(2,:);
StartY = data(3,:);
StartZ = data(4,:);
InputX = data(5,:);
InputY = data(6,:);
InputZ = data(7,:);
MeasX = data(8,:);
MeasY = data(9,:);
MeasZ = data(10,:);
AnchorPos1 = [0; 0; 0];
AnchorPos2 = [0; 3236; 0];
AnchorPos3 = [0; 1234; 7697];
SpooledOffset1 = 979;
SpooledOffset2 = 4281;
SpooledOffset3 = 2361;

Start = [StartX; StartY; StartZ];
Input = [InputX; InputY; InputZ];
Meas = [MeasX; MeasY; MeasZ];
Err = Meas - Input;
ErrEuclid = sqrt(Err(1,:).^2 + Err(2,:).^2 + Err(3,:).^2);

%% Clear temporary variables
clearvars data raw;

%% Plot Euclidean Error
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.2, 0.6, 0.4]); 
ylim([0 80]);
hold on;
grid on;
title("Euclidean Error");
h = plot(Measurement, ErrEuclid, 'rx');
set(h, 'linewidth', 1.5);
xlabel("Measurement");
ylabel("Error [mm]");
legend("Error between input and measured output");

%% Plot lengt of the different ropes
% Vector between input coordinate for Gondola and anchor position
Dist1Input = Input - AnchorPos1;
Dist2Input = Input - AnchorPos2;
Dist3Input = Input - AnchorPos3;

% length of this vector (input rope length)
InputLength1 = sqrt(Dist1Input(1,:).^2 + Dist1Input(2,:).^2 + Dist1Input(3,:).^2);
InputLength2 = sqrt(Dist2Input(1,:).^2 + Dist2Input(2,:).^2 + Dist2Input(3,:).^2);
InputLength3 = sqrt(Dist3Input(1,:).^2 + Dist3Input(2,:).^2 + Dist3Input(3,:).^2);

% Plot
figure;
hold on;
plot(Measurement, InputLength1);
plot(Measurement, InputLength2);
plot(Measurement, InputLength3);
title("Input length of the ropes");
xlabel("Measurement");
ylabel("Length of rope [mm]");
legend("Motor1", "Motor2", "Motor3", 'Location' ,'best');

%% Vector between measurement and anchor Position
Dist1Meas = Meas - AnchorPos1;
Dist2Meas = Meas - AnchorPos2;
Dist3Meas = Meas - AnchorPos3;

% length of this vector (output rope length)
OutputLength1 = sqrt(Dist1Meas(1,:).^2 + Dist1Meas(2,:).^2 + Dist1Meas(3,:).^2);
OutputLength2 = sqrt(Dist2Meas(1,:).^2 + Dist2Meas(2,:).^2 + Dist2Meas(3,:).^2);
OutputLength3 = sqrt(Dist3Meas(1,:).^2 + Dist3Meas(2,:).^2 + Dist3Meas(3,:).^2);

% Plot
figure;
hold on;
plot(Measurement, OutputLength1);
plot(Measurement, OutputLength2);
plot(Measurement, OutputLength3);
title("Measured Length of the ropes");
xlabel("Measurement");
ylabel("Length of rope [mm]");
legend("Motor1", "Motor2", "Motor3", 'Location' ,'best');

%% Error in spooling of the motors
ErrLength1 = OutputLength1 - InputLength1;
ErrLength2 = OutputLength2 - InputLength2;
ErrLength3 = OutputLength3 - InputLength3;

% Plot
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.2, 0.6, 0.4]); 
ylim([-40 40]);
hold on;
grid on;
h = plot(Measurement, ErrLength1, 'rx');
set(h, 'linewidth', 1.5);
h = plot(Measurement, ErrLength2, 'bx');
set(h, 'linewidth', 1.5);
h = plot(Measurement, ErrLength3, 'kx');
set(h, 'linewidth', 1.5);
title("Error in spoolng");
xlabel("Measurement");
ylabel("Error [mm]");
legend("Motor1", "Motor2", "Motor3", 'Location' ,'best');

figure;
hist(ErrEuclid);
