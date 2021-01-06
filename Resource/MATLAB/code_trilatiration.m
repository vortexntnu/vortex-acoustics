% parameter values for anchor node coordinates and signal speed 
a = 10 ; b = 20 ; c = 30 ; v = 300 ;  

% TDOA of Signals at Blind Node 
del_t_1 = 0.05 ; % input example                 
del_t_2 = 0.03 ; % input example 
del_t_3 = 0.02 ; % input example   

J_1 = (v.*del_t_1)./2 ; 
J_2 = (v.*del_t_2)./2 ; 
J_3 = (v.*del_t_3)./2 ;   

x = linspace(-10, 20, 10000) ;   

% Hyperbolas with side AB as transverse axis 
y_1 = sqrt(((a+b)./2)^2 - (J_1).^2).*sqrt(((x - (b-a)./2)./(J_1)).^2 - 1) ; 
y_2 = -sqrt(((a+b)./2)^2 - (J_1).^2).*sqrt(((x - (b-a)./2)./(J_1)).^2 - 1) ; 

% Hyperbolas with side AC as transverse axis 
A = 8.*b.*c.*x - 4.*c.*(b.^2 - c.^2) - 16.*c.*(J_2).^2 ; 
B = sqrt(64.*((J_2).^2).*(b.^2 + c.^2 - 4.*(J_2).^2).*(4.*x.^2 - 4.*b.*x + b.^2 + c.^2 - 4.*(J_2).^2)) ; 
C = 8.*(c.^2 - 4.*(J_2).^2) ; y_3 = (A+B)./C ; y_4 = (A-B)./C ;   

% Hyperbolas with side CB as axis 
E = -8.*a.*c.*x - 4.*c.*(a.^2 - c.^2) - 16.*c.*(J_3).^2 ; 
F = sqrt(64.*((J_3).^2).*(a.^2 + c.^2 - 4.*(J_3).^2).*(4.*x.^2 + 4.*a.*x + a.^2 + c.^2 - 4.*(J_3).^2)) ; 
G = 8.*(c.^2 - 4.*(J_3).^2) ;  

y_5 = (E+F)./G ; 
y_6 = (E-F)./G ;   

hold on 
plot(x,y_1,'r',x,y_2,'r') 
plot(x,y_3,'g',x,y_4,'g') 
plot(x,y_5,'b',x,y_6,'b') 
plot(x,0,'k',x,3.*x+30,'k',x,-(1.5).*x+30,'k')  
axis square 
axis([-10 20 0 30]) 
text(-9,0.5,'A(-10,0)') 
text(2,29,'C(0,30)') 
text(17,0.5,'B(20,0)') 
text(15.75,12.5,'P_1 (x,y)') 
text(-4.7,11.88,'P_2 (x,y)') 
hold off