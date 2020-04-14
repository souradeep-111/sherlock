% Ts = 0.1;  % Sample Time
% N = 3;    % Prediction horizon
% Duration = 10; % Simulation horizon

Ts = 1;  % Sample Time
N = 3;    % Prediction horizon
Duration = 10; % Simulation horizon

radius = 0.01;

global simulation_result;

for m=1:100

x = [-1 + radius*rand(1);
    -1 + radius*rand(1);
     9 + radius*rand(1);
     -1 + radius*rand(1);
     -1 + radius*rand(1);
     -1 + radius*rand(1);
      0;
      0;
      0;
      1;
      -1 + radius*rand(1);
      -1 + radius*rand(1);
      -1 + radius*rand(1);
      0;
      0;
      0;
      0;
      0;];  

  simulation_result = x;
  
x_now = zeros(18,1);
x_next = zeros(18,1);


x_now = x;

for ct = 1:(Duration/Ts)
    
    u = NN_output(x_now,100,1,'trial_controller_3');
      
    x_next = system_eq_dis(x_now, Ts, u);

    x = x_next;
    x_now = x_next;        
end

plot(simulation_result(18,:),simulation_result(3,:), 'color', 'r');
xlabel('x');
ylabel('y');
hold on;

m
    
end

