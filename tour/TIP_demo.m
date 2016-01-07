
% 15-463, Project 5, Tour Into the Picture
% Sample startup code by Alyosha Efros (so it's buggy!)
%
% We read in an image, get the 5 user-speficied points, and find
% the 5 rectangles.  
close all; clear; clc;

% read in sample image
im = imread('woojoo.jpg');
foreground = imread('woojoo_foreground.jpg');
[temp_y, focal_length, temp_channels] = size(im);
% Run the GUI in Figure 1
figure(1);
[vx,vy,irx,iry,orx,ory] = TIP_GUI(im);

% Find the cube faces and compute the expended image
[bim,bim_alpha,vx,vy,ceilrx,ceilry,floorrx,floorry,...
    leftrx,leftry,rightrx,rightry,backrx,backry,big_foreground] = ...
    TIP_get5rects(im,vx,vy,irx,iry,orx,ory,foreground);

% display the expended image
figure(2);
imshow(bim);

[y_max, x_max, temp_channels] = size(bim);
% Here is one way to use the alpha channel (works for 3D plots too!)
%%alpha(bim_alpha);

% Draw the Vanishing Point and the 4 faces on the image
figure(2);
hold on;
plot(vx,vy,'w*');
plot([ceilrx ceilrx(1)], [ceilry ceilry(1)], 'y-');
plot([floorrx floorrx(1)], [floorry floorry(1)], 'm-');
plot([leftrx leftrx(1)], [leftry leftry(1)], 'c-');
plot([rightrx rightrx(1)], [rightry rightry(1)], 'g-');
hold off;

% figure(99); 
% imshow(big_foreground);

x_c = max(vx-leftrx(2),rightrx(1)-vx);
x_w = max(vx, x_max-vx);

z_max = round(focal_length*x_w/x_c - focal_length);

[temp_y, temp_x] = find(big_foreground(:,:,1)~=1);
fore_depth = max(temp_y);
y_c = fore_depth-vy;
y_w = y_max-vy;

z_fore = round(focal_length*y_w/y_c - focal_length);

% initialize empty faces
channel = 3;    % RGB color channel
back_face = zeros(y_max, x_max, channel);
top_face = zeros(z_max, x_max, channel);
bottom_face = zeros(z_max, x_max, channel);
left_face = zeros(y_max, z_max, channel);
right_face = zeros(y_max, z_max, channel);
fore_face = zeros(y_max, x_max, channel);

% 3D coordinates relative to vanishing point
xw_min = 1-vx;
xw_max = x_max-vx;
xw = xw_min:xw_max;
yw_min = 1-vy;
yw_max = y_max-vy;
yw = yw_min:yw_max;
zw_min = 1;
zw_max = z_max;
zw = zw_min:zw_max;

% foreground interpolation
display('foreground object interpolating...');
temp_den = z_fore/focal_length+1;
for i=1:y_max
   yc = yw(i)/temp_den+vy;
   B = abs(yc-floor(yc));
   for j=1:x_max
       xc = xw(j)/temp_den+vx;
       A = abs(xc-floor(xc));
       m = big_foreground(floor(yc),floor(xc),:);
       n = big_foreground(floor(yc),floor(xc)+1,:);
       o = big_foreground(floor(yc)+1,floor(xc),:);
       p = big_foreground(floor(yc)+1,floor(xc)+1,:);
       v1 = (1-A)*m + A*n;
       v2 = (1-A)*o + A*p;
       fore_face(i,j,:) = (1-B)*v1+B*v2;
   end
end
figure(13); hold on;
% imshow(fore_face);
subplot(3,2,1), subimage(fore_face);
title('foreground object');
axis off;

% back face
display('back face interpolating...');
temp_den = zw_max/focal_length+1;
for i=1:y_max
   yc = yw(i)/temp_den+vy;
   B = abs(yc-floor(yc));
   for j=1:x_max
       xc = xw(j)/temp_den+vx;
       A = abs(xc-floor(xc));
       m = bim(floor(yc),floor(xc),:);
       n = bim(floor(yc),floor(xc)+1,:);
       o = bim(floor(yc)+1,floor(xc),:);
       p = bim(floor(yc)+1,floor(xc)+1,:);
       v1 = (1-A)*m + A*n;
       v2 = (1-A)*o + A*p;
       back_face(i,j,:) = (1-B)*v1+B*v2;
   end
end
% figure(3);
% imshow(back_face);
subplot(3,2,2), subimage(back_face);
axis off;
title('back');

%%
% top face
display('top face interpolating...');
for i=1:z_max
    temp_den = 1+zw(i)/focal_length;
    yc = yw_min/temp_den+vy;
    B = abs(yc-floor(yc));
    for j=1:x_max
        xc = xw(j)/temp_den+vx;
        A = abs(xc-floor(xc));
        m = bim(floor(yc),floor(xc),:);
        n = bim(floor(yc),floor(xc)+1,:);
        o = bim(floor(yc)+1,floor(xc),:);
        p = bim(floor(yc)+1,floor(xc)+1,:);
        v1 = (1-A)*m + A*n;
        v2 = (1-A)*o + A*p;
        top_face(i,j,:) = (1-B)*v1+B*v2;        
    end
end
% figure(4);
% imshow(top_face);
subplot(3,2,3), subimage(top_face);
axis off;
title('top');


% bottom face
display('bottom face interpolating...');
for i=1:z_max
    temp_den = 1+zw(i)/focal_length;
    yc = yw_max/temp_den+vy;
    B = abs(yc-floor(yc));
    for j=1:x_max
        xc = xw(j)/temp_den+vx;
        A = abs(xc-floor(xc));
        m = bim(floor(yc),floor(xc),:);
        n = bim(floor(yc),floor(xc)+1,:);
        o = bim(floor(yc)+1,floor(xc),:);
        p = bim(floor(yc)+1,floor(xc)+1,:);
        v1 = (1-A)*m + A*n;
        v2 = (1-A)*o + A*p;
        bottom_face(z_max-i+1,j,:) = (1-B)*v1+B*v2;        
    end
end
% figure(5);
% imshow(bottom_face);
subplot(3,2,4), subimage(bottom_face);
axis off;
title('bottom');

% left face
display('left face interpolating...');
for i=1:y_max
    for j=1:z_max
        temp_den = 1+zw(j)/focal_length;
        xc = xw_min/temp_den+vx;
        yc = yw(i)/temp_den+vy;
        A = abs(xc-floor(xc));
        B = abs(yc-floor(yc));
        m = bim(floor(yc),floor(xc),:);
        n = bim(floor(yc),floor(xc)+1,:);
        o = bim(floor(yc)+1,floor(xc),:);
        p = bim(floor(yc)+1,floor(xc)+1,:);
        v1 = (1-A)*m + A*n;
        v2 = (1-A)*o + A*p;
        left_face(i,j,:) = (1-B)*v1+B*v2;        
    end
end
% figure(6);
% imshow(left_face);
subplot(3,2,5), subimage(left_face);
axis off;
title('left');

% right face
display('right face interpolating...');
for i=1:y_max
    for j=1:z_max
        temp_den = 1+zw(j)/focal_length;
        xc = xw_max/temp_den+vx;
        yc = yw(i)/temp_den+vy;
        A = abs(xc-floor(xc));
        B = abs(yc-floor(yc));
        m = bim(floor(yc),floor(xc),:);
        n = bim(floor(yc),floor(xc)+1,:);
        o = bim(floor(yc)+1,floor(xc),:);
        p = bim(floor(yc)+1,floor(xc)+1,:);
        v1 = (1-A)*m + A*n;
        v2 = (1-A)*o + A*p;
        right_face(i,z_max-j+1,:) = (1-B)*v1+B*v2;        
    end
end
% figure(7);
% imshow(right_face);
subplot(3,2,6), subimage(right_face);
axis off;
title('right');
hold off;

% display box
display('display box');
figure(8); hold on;
% bottom
surface([0 x_max; 0 x_max], [0 0; z_max z_max], [0 0; 0 0], ...
    'FaceColor', 'texturemap', 'CData', flip(bottom_face,1),'EdgeAlpha',0);
% top
surface([0 x_max; 0 x_max], [0 0; z_max z_max], [y_max y_max; y_max y_max], ...
    'FaceColor', 'texturemap', 'CData', top_face, 'EdgeAlpha',0);
% back
surface([0 x_max; 0 x_max], [z_max z_max; z_max z_max], [0 0; y_max y_max], ...
    'FaceColor', 'texturemap', 'CData', flip(back_face,1),'EdgeAlpha',0);
% left
surface([0 0; 0 0], [0 z_max; 0 z_max], [0 0; y_max y_max], ...
    'FaceColor', 'texturemap', 'CData', flip(left_face,1),'EdgeAlpha',0);
% right
surface([x_max x_max; x_max x_max], [0 z_max; 0 z_max], [0 0; y_max y_max], ...
    'FaceColor', 'texturemap', 'CData', flip(flip(right_face,1),2),'EdgeAlpha',0);



% foreground object
logical = (fore_face(:,:,1)~=1).*255;
foreground_handle = surface([0 x_max; 0 x_max], [z_fore z_fore; z_fore z_fore], [0 0; y_max y_max], ...
    'FaceColor', 'texturemap', 'CData', flip(fore_face,1),'EdgeAlpha',0);

foreground_handle.FaceAlpha = 'texturemap';
foreground_handle.AlphaData = flip(logical,1);

xlabel('width');
ylabel('depth');
zlabel('height');
view(3);

axis equal;
axis vis3d;
axis off;
camproj('perspective');
hold off;