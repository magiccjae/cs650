close all; clear; clc;
cdata = flip( imread('byu.ppm'), 1 );
cdatar = flip( cdata, 2 );

% bottom
surface([0 1; 0 1], [0 0; 1 1], [0 0; 0 0], ...
    'FaceColor', 'texturemap', 'CData', flip(cdata,1));
% bottom
% surface([-1 1; -1 1], [-1 -1; 1 1], [-1 -1; -1 -1], ...
%     'FaceColor', 'texturemap', 'CData', flip(cdata,1));
% % top
% surface([-1 1; -1 1], [-1 -1; 1 1], [1 1; 1 1], ...
%     'FaceColor', 'texturemap', 'CData', cdata );
% % font
% surface([-1 1; -1 1], [-1 -1; -1 -1], [-1 -1; 1 1], ...
%     'FaceColor', 'texturemap', 'CData', cdata );
% % back
% surface([-1 1; -1 1], [1 1; 1 1], [-1 -1; 1 1], ...
%     'FaceColor', 'texturemap', 'CData', cdatar );
% % left
% surface([-1 -1; -1 -1], [-1 1; -1 1], [-1 -1; 1 1], ...
%     'FaceColor', 'texturemap', 'CData', cdatar );
% % right
% surface([1 1; 1 1], [-1 1; -1 1], [-1 -1; 1 1], ...
%     'FaceColor', 'texturemap', 'CData', cdata );
figure(1);
xlabel('x');
ylabel('y');
view(3);
