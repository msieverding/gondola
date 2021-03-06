%% Import data from spreadsheet
% Script for importing data from the following spreadsheet:
%
%    Workbook: C:\Users\Marvin\Dropbox\01 Studium\III WiSe 17 18\01 Projektarbeit\Messungen\2017_09_21\2017_09_21.xls
%    Worksheet: 02
%
% To extend the code for use with different selected data or a different
% spreadsheet, generate a function instead of a script.

% Auto-generated by MATLAB on 2017/09/21 13:42:19

clearvars;
%% Import the data
[~, ~, raw] = xlsread('C:\Users\Marvin\Desktop\Projektarbeit\gondola\doc\Measurement_Matlab\2017_09_21\2017_09_21.xls','02');
raw = raw(2:13,1:4);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
Length = data(:,1);         % Input length
MeasM1 = data(:,2);         % Motor 1 measured without any corrections
MeasCorrM1 = data(:,3);     % Motor 1 with correction data derived below
Offset = 1132;                              % mm between motor and anchor
ErrM1 = MeasM1 - Length;                        % Too much spooled mm's
ErrCorrM1 = MeasCorrM1 - Length;                % Too much spooled mm's with applied correction

TotalLength = 10000;                             % Approx. 10 Meter on spool
RopeOnSpoolM1 = TotalLength - Offset - MeasM1;       % mm rope spooled on spool
RopeOnSpoolCorrM1 = TotalLength - Offset - MeasCorrM1;       % mm rope spooled on spool

% Data below was generated with motor 2 but coefficients calculated below
% with data from motor 1
MeasCorrM2 = data(:,4);     % Motor 2 with correction data derived below from motor1
ErrCorrM2 = MeasCorrM2 - Length;

%% Clear temporary variables
clearvars data raw;
close all;

%% Plot Error against length for both motors
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]);
hold on;
plot(Length, ErrM1, 'rx');
plot(Length, ErrCorrM1, 'gx');
plot(Length, ErrCorrM2, 'bx');
title("Error against input length");
xlabel("Input length [mm]");
ylabel("Error [mm]");
legend("Error without correction (M1)", "Error with implemented estimation (M1)", "Error with implemented Estimation (M2)", 'Location' ,'best');
% Reult: Error of motor 1 looks good. But Error of motor 2 is not good. It
% looks like the error is not linear over the whole distance.

%% Plot Error against rope on spool
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]); ;
hold on;
plot(RopeOnSpoolM1, ErrM1, 'r');
plot(RopeOnSpoolCorrM1, ErrCorrM1, 'g');
title("Error against spooled rope");
xlabel("Rope on spool [mm]");
ylabel("Error [mm]");
legend("Error without correction", "Error with implemented estimation", 'Location' ,'best');
% Das geht so nicht, wie ich am 22.09.17 rausgefunden habe. Denn man sieht
% in dem einen Plot ganz gut, dass man nicht approximieren kann, wie sich
% das verh�lt, wenn man mehr rope hinzuf�gt.

%% Regression of Error against rope on spool
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]); ;
hold on;
x = RopeOnSpoolM1;
y = ErrM1;
X = [ones(length(x), 1) x];
m = X\y; % least squares regression % attention: these are mm not cm!
m./10                               % output data in cm
ErrRegression = X * m;              % estimated error 
plot(x, y, 'rx');
plot(x, ErrRegression, 'g-');
title("Linear regression of error");
xlabel("Rope on spool [mm]");
ylabel("Error [mm]");
legend("error", "estimation of error", 'Location' ,'best');

%% Regression of Error against real spooled rope Version 1
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]); ;
hold on;
x = TotalLength - RopeOnSpoolM1;
y = ErrM1;
m = x\y; % least squares regression % attention: these are mm not cm!
m./10                               % output data in cm
ErrRegression = x * m;              % estimated error 
plot(x, y, 'rx');
plot(x, ErrRegression, 'g-');
title("Linear regression of error V1");
xlabel("Spooled rope [mm]");
ylabel("Error [mm]");
legend("error", "estimation of error", 'Location' ,'best');

%% Regression of Error against real spooled rope Version 2
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.1, 0.7, 0.7]); ;
hold on;
x = TotalLength - RopeOnSpoolM1;
y = ErrM1;
X = [ones(length(x), 1) x];
m = X\y; % least squares regression % attention: these are mm not cm!
m./10                               % output data in cm
ErrRegression = X * m;              % estimated error 
plot(x, y, 'rx');
plot(x, ErrRegression, 'g-');
title("Linear regression of error V2");
xlabel("Spooled rope [mm]");
ylabel("Error [mm]");
legend("error", "estimation of error", 'Location' ,'best');