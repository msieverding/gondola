%% Import data from spreadsheet
% Script for importing data from the following spreadsheet:
%
%    Workbook: C:\Users\Marvin\Dropbox\01 Studium\III WiSe 17 18\01 Projektarbeit\Messungen\2017_09_22\2017_09_22.xls
%    Worksheet: 02
%
% To extend the code for use with different selected data or a different
% spreadsheet, generate a function instead of a script.

% Auto-generated by MATLAB on 2017/09/22 12:16:21

clearvars;
%% Import the data from first measurement
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_09_22\2017_09_22.xls','01');
raw = raw(4:end,1:2);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
Length = data(:,1);
Meas = data(:,2);
Err = Meas - Length;
RopeTotalLength = 10000;
RopeOnSpool = RopeTotalLength - Meas;

%% Clear temporary variables
clearvars data raw;

%% Import the data from corrected measurement
% Data generated with 1D measurements with corrections from measurement 01
% The quadratic error estimation depended on spooled length
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_09_22\2017_09_22.xls','02');
raw = raw(4:end,1:2);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
%LengthC = data(:,1); same as above!
MeasCorr = data(:,2);
ErrCorr = MeasCorr- Length;


%% Clear temporary variables
clearvars data raw;
close all;

%% Plot Error against input length
figure;
hold on;
plot(Length, Err, 'rx');
plot(Length, ErrCorr, 'gx');
title("Error against input length");
xlabel("Input length [mm]");
ylabel("Error [mm]");
legend("Error without correction", "Error with quadratic correction", 'Location' ,'best');

