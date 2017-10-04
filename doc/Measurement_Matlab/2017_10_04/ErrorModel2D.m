clearvars;

dpos = 20;          % error in mm (+)
dneg = 20;          % error in mm (-)
r1 = 2000;          % length of rope 1
r2 = 2000;          % length of rope 2
AnchorYOffset = 3236;

r1d = linspace(r1-dneg, r1+dpos, dneg + dpos + 1)';
r2d = linspace(r2-dneg, r2+dpos, dneg + dpos + 1)';

%idx1 = reshape(repmat(1:length(r1d), length(r2d), 1), 1, []); 
%idx2 = repmat(1:length(r2d), 1, length(r1d));
%combinations = [r1d(idx1), r2d(idx2)];

% combining the edges is enough to get the rectangle
combinations = [r1+dpos r2+dpos; r1+dpos r2-dneg; r1-dneg r2+dpos; r1-dneg r2-dneg];

y = (combinations(:,1).^2 - combinations(:,2).^2 + AnchorYOffset^2)/(2*AnchorYOffset);
x = sqrt((combinations(:,1).^2 - y.^2));

y_real = (r1.^2 - r2.^2 + AnchorYOffset^2)/(2*AnchorYOffset);
x_real = sqrt(r1.^2 - y_real.^2);

[x, sortx] = sort(x);
y = y(sortx);

close all;
figure;
hold on;
title("2D error with given 1D error");
set(gca,'YDir','reverse');      % reverse direction of y axis
xlabel("Y [mm]");
ylabel("X [mm]");
plot(y_real, x_real, 'gx');
plot(y, x, 'ro');
legend("target position", "edges of defective position", 'Location' ,'best');
x_max = max(x);
x_min = min(x);
y_max = max(y);
y_min = min(y);
x_err_max = x_max - x_real;
x_err_min = x_real - x_min;
y_err_max = y_max - y_real;
y_err_min = y_real - y_min;