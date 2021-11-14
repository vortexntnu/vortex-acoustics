

%position of 4 hydrophones
% if al xi or al yi are on the same plane, then A_T*A will be a singular matrix. 
x0 = -0.11; x1 = 0.11; x2 = 0.0; x3 = 0.7; 
y0 = 0.31; y1 =0.31; y2 = -0.24; y3 = 0.7; 

%this is not part of the code, only to simulate the TDAO array 
xs = 10.0; ys = 2.0; 

d0= sqrt((xs-x0).^2 +(ys-y0).^2); 
d1= sqrt((xs-x1).^2 +(ys-y1).^2);
d2= sqrt((xs-x2).^2 +(ys-y2).^2);
d3= sqrt((xs-x3).^2 +(ys-y3).^2);

disp("fasit x,y,d0"); disp(xs) ; disp(ys); disp(d0) 

d01 = d0-d1; d02 = d0-d2; d03 = d0-d3; %TDOA array

b1 = 0.5*(x0.^2-x1.^2+y0.^2-y1.^2+d01.^2); 
b2 = 0.5*(x0.^2-x2.^2+y0.^2-y2.^2+d02.^2);
b3 = 0.5*(x0.^2-x3.^2+y0.^2-y3.^2+d03.^2);

b = [b1; b2; b3]; 

A = [x0-x1 y0-y1 d01; x0-x2 y0-y2 d02; x0-x3 y0-y3 d03]; 
disp("A"); disp(A)

A_T = transpose(A);
disp("A_T"); disp(A_T) 

temp = A_T*A; 
disp("A_T*A"); disp(temp)
temp_inv = inv(temp); 
disp("temp_inv"); disp(temp_inv)

x = temp_inv*A_T*b; 
disp("x, y, d0"); disp(x)

e = b-A*x; 
disp("error"); disp(e); 
