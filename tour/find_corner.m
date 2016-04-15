% given a line thru (vx,vy) and (rx,ry), find where it should end given the
% image edges limitx and limity
function [x,y] = find_corner(vx,vy,rx,ry,limitx,limity);

y1 = limity;
x1 = find_line_x(vx,vy,rx,ry,limity);   % x intercept
x2 = limitx;
y2 = find_line_y(vx,vy,rx,ry,limitx);   % y intercept
if (sum(([vx vy]-[x1 y1]).^2) > sum(([vx vy]-[x2 y2]).^2)), % a point with longer distance from vanishing point selected
  x = x1;
  y = y1;
else
  x = x2;
  y = y2;
end;