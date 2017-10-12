clearvars;
close all;

Coordinate = [400 1500 2000];
AnchorPos1 = [0; 0; 0];
AnchorPos2 = [0; 3236; 0];
AnchorPos3 = [0; 1234; 7697];

dpos = 20;          % error in mm (+)
dneg = 20;          % error in mm (-)
% length of ropes
r1 = sqrt((AnchorPos1(1) - Coordinate(1))^2 + (AnchorPos1(2) - Coordinate(2))^2 + (AnchorPos1(3) - Coordinate(3))^2);
r2 = sqrt((AnchorPos2(1) - Coordinate(1))^2 + (AnchorPos2(2) - Coordinate(2))^2 + (AnchorPos2(3) - Coordinate(3))^2);
r3 = sqrt((AnchorPos3(1) - Coordinate(1))^2 + (AnchorPos3(2) - Coordinate(2))^2 + (AnchorPos3(3) - Coordinate(3))^2);

% only used for formulas
Anchor2YOffset = 3236;
Anchor3YOffset = 1234;
Anchor3ZOffset = 7697;

% combining the edges is enough
combinations = [r1+dpos r2+dpos r3+dpos;
                r1+dpos r2+dpos r3-dneg;
                r1+dpos r2-dneg r3+dpos;
                r1+dpos r2-dneg r3-dneg;
                r1-dneg r2+dpos r3+dpos;
                r1-dneg r2+dpos r3-dneg;
                r1-dneg r2-dneg r3+dpos;
                r1-dneg r2-dneg r3-dneg];

% TODO das minus in der wurzel bei x gehört da mathematisch nicht hin. Das
% Ergebnis sieht aber plausibel aus. Wo habe ich mich verrechnet
% TODO why are there some complex numbers in X?
y = (combinations(:,1).^2 - combinations(:,2).^2 + 10471696) / 6472;
z = (combinations(:,2).^2 - combinations(:,3).^2 + 4004 * y + 50294869) / 15394;
x = sqrt((combinations(:,3).^2 - (y-1234).^2 - (z-7697).^2));

y_real = (r1.^2 - r2.^2 + 10471696) / 6472;
z_real = (r2.^2 - r3.^2 + 4004 * y_real + 50294869) / 15394;
x_real = sqrt((r3.^2 - (y_real-1234).^2 - (z_real-7697).^2));


figure;
plot3(Coordinate(2), Coordinate(1), Coordinate(3), 'gx');
hold on;
%plot3(y_real, x_real, z_real, 'gx'); position calculated with calculation
%for the other points. Just to check it
plot3(y, x, z, 'ro');
title("3D error with given 1D error");
grid on;
axis on;
set(gca,'YDir','reverse');      % reverse direction of y axis
xlabel("Y [mm]");
ylabel("X [mm]");
zlabel("Z [mm]");
legend("target position", "corners of defective position", 'Location' ,'best');

%% Plot as figure
% r1-r2 plane with r3+
idx = [1 3 7 5 1];
% r1-r2 plane with r3-
idx = [idx 2 4 8 6 2];
% r1-r3 plane with r2+
idx = [idx 1 2 6 5 1];
% r1-r3 plane with r2-
idx = [idx 3 4 8 7 3];
figure;
plot3(y_real, x_real, z_real, 'gx');
hold on;
plot3(y(idx), x(idx), z(idx), 'r');
title("3D error with given 1D error (approx. corners)");
grid on;
axis on;
set(gca,'YDir','reverse');      % reverse direction of y axis
xlabel("Y [mm]");
ylabel("X [mm]");
zlabel("Z [mm]");
legend("target position", "edges of defective position", 'Location' ,'best');


%% Plot points inside
gridintensity = 0.25;
r1d = linspace(r1-dneg, r1+dpos, (dneg + dpos + 1) * gridintensity)';
r2d = linspace(r2-dneg, r2+dpos, (dneg + dpos + 1) * gridintensity)';
r3d = linspace(r3-dneg, r3+dpos, (dneg + dpos + 1) * 0.5 * gridintensity)';
[r1d ,r2d ,r3d] = meshgrid(r1d, r2d, r3d);

r1d = r1d(:);
r2d = r2d(:);
r3d = r3d(:);
y_all = (r1d.^2 - r2d.^2 + 10471696) / 6472;
z_all = (r2d.^2 - r3d.^2 + 4004 * y_all + 50294869) / 15394;
x_all = sqrt((r3d.^2 - (y_all-1234).^2 - (z_all-7697).^2));
x_all = real(x_all); 

f = figure;
set(f, 'Units', 'normalized', 'Position', [0.2, 0.2, 0.3, 0.4]);
h = plot3(y(idx), x(idx), z(idx), 'r-');
set(h, 'linewidth', 1.5);
hold on;
scatter3(y_all(:), x_all(:), z_all(:), 'k.');
h = plot3(y_real, x_real, z_real, 'gx');
set(h, 'linewidth', 1.5);
title("3D error with given 1D error (grid)");
grid on;
axis on;
set(gca,'YDir','reverse');      % reverse direction of y axis
xlabel("Y [mm]");
ylabel("X [mm]");
zlabel("Z [mm]");
legend("edges of defective position", "grid of defective position", "target position", 'Location' ,'best');

%% Maximum error per dimension
x_err_max = max(x_all) - x_real;
x_err_min = x_real - min(x_all);
y_err_max = max(y_all) - y_real;
y_err_min = y_real - min(y_all);
z_err_max = max(z_all) - z_real;
z_err_min = z_real - min(z_all);

%% Maximum euclidean error
euclidean_err = sqrt((x_all - x_real).^2 + (y_all - y_real).^2 + (z_all - z_real).^2);
euclidean_err_max = max(euclidean_err);
