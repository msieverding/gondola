clearvars;

dpos = 20;          % error in mm (+)
dneg = 20;          % error in mm (-)
r1 = 2000;          % length of rope 1
r2 = 2000;          % length of rope 2
r3 = 2000;
% only used for formulas
Anchor1YOffset = 3236;
Anchor2YOffset = 1234;
Anchor2ZOffset = 7697;

r1d = linspace(r1-dneg, r1+dpos, dneg + dpos + 1)';
r2d = linspace(r2-dneg, r2+dpos, dneg + dpos + 1)';

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
y = (combinations(:,1).^2 - combinations(:,2).^2 + 10471696) / 6472;
z = (combinations(:,2).^2 - combinations(:,3).^2 + 4004 * y + 50294869) / 15394;
x = sqrt(-(combinations(:,3).^2 - (y-1234).^2 - (z-7697).^2));

y_real = (r1.^2 - r2.^2 + 10471696) / 6472;
z_real = (r2.^2 - r3.^2 + 4004 * y_real + 50294869) / 15394;
x_real = sqrt(-(r3.^2 - (y_real-1234).^2 - (z_real-7697).^2));


close all;
hold off;
plot3(y, x, z, 'ro');
hold on;
title("3D error with given 1D error");
grid on;
axis on;
set(gca,'YDir','reverse');      % reverse direction of y axis
xlabel("Y [mm]");
ylabel("X [mm]");
zlabel("Z [mm]");
plot3(y_real, x_real, z_real, 'gx');
legend("target position", "edges of defective position", 'Location' ,'best');
x_err_max = max(x) - x_real;
x_err_min = x_real - min(x);
y_err_max = max(y) - y_real;
y_err_min = y_real - min(y);
z_err_max = max(z) - z_real;
z_err_min = z_real - min(z);