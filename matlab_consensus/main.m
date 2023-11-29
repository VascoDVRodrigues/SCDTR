close all, clear all, clc;

%EXPERIMENTAL CASES
% case 1
%L1 = 150; o1 = 30; L2 = 80; o2 = 0;
% case 2
L1 = 10; o1 = 0.52; L2 = 10; o2 = 0.52; L3=10; o3=1.23;
% case 3
%L1 = 80; o1 = 50; L2 = 270; o2 = 50;

%COST FUNCTION PARAMETERS
%symmetric costs
c1 = 1; c2 = 1; c3 = 10;
%asymmetric costs
%c1 = 1; c2 = 3;

%SOLVER PARAMETERS
rho = 0.07;
maxiter = 100;

%SYSTEM CALIBRATION PARAMETERS
k11 = 0.06588*40.96; k12 = 0.00206*40.96;k13 = 0.00627*40.96; k21 = 0.00123*40.96; k22 = 0.00895*40.96;k23=0.00356*40.96;
k31 = 0.00473*40.96; k32 = 0.00517*40.96; k33 = 0.01482*40.96;


%VARIABLES FOR CENTRALIZED SOLUTION
K = [k11, k12 ,k13; k21, k22, k23; k31, k32, k33];
c = [c1;c2;c3]; 
L = [L1;L2;L3]; 
o = [o1;o2;o3];

%VARIABLES FOR STORING THE HISTORY OF THE DISTRIBUTED SOLUTION
d11 = zeros(1,maxiter);
d12 = zeros(1,maxiter);
d21 = zeros(1,maxiter);
d22 = zeros(1,maxiter);
av1 = zeros(1,maxiter);
av2 = zeros(1,maxiter);

%DISTRIBUTED NODE INITIALIZATION

%node1
node1.index = 1;
node1.d = [0;0;0];
node1.d_av = [0;0;0];
node1.y = [0;0;0];
node1.k = [k11;k12;k13]; 
node1.n = norm(node1.k)^2;
node1.m = node1.n-k11^2;
node1.c = [c1;0;0];
node1.o = o1;
node1.L = L1;

%node 2
node2.index = 2;
node2.d = [0;0;0];
node2.d_av = [0;0;0];
node2.y = [0;0;0];
node2.k = [k21;k22;k23]; 
node2.n = norm(node2.k)^2;
node2.m = node2.n-k22^2;
node2.c = [0;c2;0];
node2.o = o2;
node2.L = L2;

%node3
node3.index = 3;
node3.d = [0;0;0];
node3.d_av = [0;0;0];
node3.y = [0;0;0];
node3.k = [k31;k32;k33]; 
node3.n = norm(node3.k)^2;
node3.m = node3.n-k33^2;
node3.c = [0;0;c3];
node3.o = o3;
node3.L = L3;


%DISTRIBUTED SOLVER

%Initial condition (iteration = 1)
d11(1) = node1.d(1);
d12(1) = node1.d(2);
d21(1) = node2.d(1);
d22(1) = node2.d(2);
av1(1) = (d11(1)+d21(1))/2;
av2(1) = (d12(1)+d22(1))/2;

%iterations
for i=2:maxiter
   %COMPUTATION OF THE PRIMAL SOLUTIONS
   % node 1
   [d1, cost1] = consensus_iterate(node1, rho);
   node1.d = d1;
   disp(node1.d);
   %DEBUG: check with matlab quadprog
   %Q_ = [rho, 0; 0 rho];
   %c_ = c+node1.y - rho*node1.d_av;
   %A_ = [-k11 -k12; -1 0; 1 0];
   %b_ = [o1-L1, 0, 100];
   %d1_ = quadprog(Q_,c_,A_,b_,[],[],[],[], zeros(2,1));
   
   %node2
   [d2, cost2] = consensus_iterate(node2, rho);   
   node2.d = d2;
   %disp(node2.d);
   %DEBUG: check with matlab quadprog
   %Q_ = [rho, 0; 0 rho];
   %c_ = c+node2.y - rho*node2.d_av;
   %A_ = [-k21 -k22;0 -1; 0 1];
   %b_ = [o2-L2, 0, 100];
   %d2_ = quadprog(Q_,c_,A_,b_,[],[],[],[], zeros(2,1));
 
   % NODES EXCHAGE THEIR SOLUTIONS
   % (COMMUNICATIONS HERE)
   %node3
   [d3, cost3] = consensus_iterate(node3, rho);   
   node3.d = d3;
   %disp(node3.d);
   %COMPUTATION OF THE AVERAGE
   %node 1
   node1.d_av = (node1.d+node2.d+node3.d)/3;
   %node 2
   node2.d_av = (node1.d+node2.d+node3.d)/3;
   %node 3
   node3.d_av = (node1.d+node2.d+node3.d)/3;
   
   
   %COMPUTATION OF THE LAGRANGIAN UPDATES
   %node 1
   node1.y = node1.y + rho*(node1.d-node1.d_av);  
   %node 2
   node2.y = node2.y + rho*(node2.d-node2.d_av);
   %node 3
   node3.y = node3.y + rho*(node3.d-node3.d_av);
   
   %SAVING DATA FOR PLOTS
   d11(i) = node1.d(1);
   d12(i) = node1.d(2);
   d21(i) = node2.d(1);
   d22(i) = node2.d(2);
   av1(i) = (d11(i)+d21(i))/2;
   av2(i) = (d12(i)+d22(i))/2;
end;

% CENTRALIZED SOLVER: USIGN MATLAB LINPROG
A_ = -K; 
b_ = [o1-L1; o2-L2;o3-L3];
c_ = c;
lb = [0;0;0]; ub = [100;100;100];
d_ = linprog(c_,A_,b_,[],[],lb,ub);
l_ = K*d_+o;

%DISPLAY DATA
disp('Matlab solutions')
d_
l_

disp('Consensus Solutions')
d = node1.d_av
l = K*d+o

%Plots
figure(10);
plot(1:50, av1, 1:50, av2, 1:50, d11, 1:50, d12, 1:50, d21, 1:50, d22);
legend('av1','av2', 'd11', 'd12', 'd21', 'd22');
title('time convergence');
xlabel('iter');

figure(15);
t = 0:100;
constr1 = (L1-o1)/k12-(k11/k12)*t;
constr2 = (L2-o2)/k22-(k21/k22)*t;
[x,y]=meshgrid(t,t);
hold on;
z = c1*x+c2*y;
contour(x,y,z);
plot(t,constr1,t,constr2,'LineWidth',2);
plot(t,zeros(size(t)),'k','LineWidth',2);
plot(zeros(size(t)),t,'k','LineWidth',2);
plot(t,100*ones(size(t)),'k','LineWidth',2);
plot(100*ones(size(t)),t,'k','LineWidth',2);
plot(av1,av2,'b--o','LineWidth',2);
%plot(av1,av2,'bo');
plot(d11,d12,'m-.x','LineWidth',1);
% plot(d11,d12,'mx');
plot(d21,d22,'k-.+','LineWidth',1);
% plot(d21,d22,'kx');
legend('cost','c1', 'c2', 'c3', 'c4', 'c5', 'c6', 'av', 'd1', 'd2');
title('solution space convergence');
xlabel('d_1');
ylabel('d_2');
plot(d_(1),d_(2),'r*')
axis([-10,110,-10,110]);
hold off;





