Ts = 0.2;  % Sample Time
N = 3;    % Prediction horizon
Duration = 6; % Simulation horizon

% For a usual control 0.1 , 5, 20

% Things specfic to the model starts here 

% Limits of initial [-1,1]^2

global simulation_result;

for m=1:1
% Setting the initial state

x0 = 0.5 + 0.4*rand(1);
y0 = 0.5 + 0.4*rand(1);
x0 = 0.9;
y0 = 0.9;

x = [x0;y0;];

simulation_result = x;

% Things specfic to the model  ends here

options = optimoptions('fmincon','Algorithm','sqp','Display','none');
uopt = zeros(N,1);

u_max = 0;

% Apply the control input constraints
LB = -2*ones(N,1);
UB = 2*ones(N,1);

x_now = zeros(2,1);
x_next = zeros(2,1);
z = zeros(3,1);

x_now = x;

% Start simulation

for ct = 1:(Duration/Ts)
     u = NN_output(x_now,4,1,'neural_network_controller')
     
     z(1) = x_now(1) ;
     z(2) = x_now(2) ;
     z(3) = u ;
     
      
%     x_next = system_eq_NN(x_now, Ts, u);
      x_next = system_eq_dis(x_now, Ts, u);

    x = x_next;
    x_now = x_next;
% Save plant states for display.
%    xHistory = [xHistory x]; 


%     Printing stuff

%    disp('x(1) = '); x(1)
%    disp('x(2) = '); x(2)

%    disp('Control input = '); u
    
        
end

% fclose(file);

%figure;
plot(simulation_result(1,:),simulation_result(2,:), 'color', 'r');
xlabel('x');
ylabel('y');
hold on;

end

% subplot(2,1,1);
% plot(0:Ts:Duration,xHistory(1,:));
% xlabel('time');
% ylabel('x');
% 
% subplot(2,1,2);
% plot(0:Ts:Duration,xHistory(2,:));
% xlabel('time');
% ylabel('y');
