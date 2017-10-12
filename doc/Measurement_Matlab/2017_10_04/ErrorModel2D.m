clearvars;

Coordinate = [400 1500];
AnchorPos1 = [0; 0];
AnchorPos2 = [0; 3236];

dpos = 20;          % error in mm (+)
dneg = 20;          % error in mm (-)
% length of ropes
% length of ropes
r1 = sqrt((AnchorPos1(1) - Coordinate(1))^2 + (AnchorPos1(2) - Coordinate(2))^2);
r2 = sqrt((AnchorPos2(1) - Coordinate(1))^2 + (AnchorPos2(2) - Coordinate(2))^2);

Anchor2YOffset = 3236;

gridintensity = 0.5;
r1d = linspace(r1-dneg, r1+dpos, (dneg + dpos + 1) * gridintensity)';
r2d = linspace(r2-dneg, r2+dpos, (dneg + dpos + 1) * gridintensity)';
idx1 = reshape(repmat(1:length(r1d), length(r2d), 1), 1, []); 
idx2 = repmat(1:length(r2d), 1, length(r1d));
combinations_all = [r1d(idx1), r2d(idx2)];

% combining the edges is enough to get the square
combinations = [r1+dpos r2+dpos; r1+dpos r2-dneg; r1-dneg r2-dneg; r1-dneg r2+dpos];

y = (combinations(:,1).^2 - combinations(:,2).^2 + Anchor2YOffset^2)/(2*Anchor2YOffset);
x = sqrt((combinations(:,1).^2 - y.^2));

y_all = (combinations_all(:,1).^2 - combinations_all(:,2).^2 + Anchor2YOffset^2)/(2*Anchor2YOffset);
x_all = sqrt((combinations_all(:,1).^2 - y_all.^2));

y_real = (r1.^2 - r2.^2 + Anchor2YOffset^2)/(2*Anchor2YOffset);
x_real = sqrt(r1.^2 - y_real.^2);


close all;
f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.2, 0.3, 0.4]);
hold on;
title("2D error with given 1D error");
set(gca,'YDir','reverse');      % reverse direction of y axis
xlabel("Y [mm]");
ylabel("X [mm]");

%plot(y_real, x_real, 'gx');
h = plot([y; y(1)], [x; x(1)], 'r--'); % Add first point to the end, to draw last line of polygon
set(h, 'linewidth', 1.5);
plot(y_all, x_all, 'k.');
h = plot(Coordinate(2), Coordinate(1), 'gx');
set(h, 'linewidth', 1.5);
legend("edges of defective position", "grid of defective position", "target position", 'Location' ,'best');

%% Maximum error per dimension
x_err_max = max(x_all) - x_real;
x_err_min = x_real - min(x_all);
y_err_max = max(y_all) - y_real;
y_err_min = y_real - min(y_all);

%% Maximum euclidean error
euclidean_err = sqrt((x_all - x_real).^2 + (y_all-y_real).^2);
euclidean_err_max = max(euclidean_err);