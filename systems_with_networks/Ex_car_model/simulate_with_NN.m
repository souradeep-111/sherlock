Ts = 0.2;  % Sample Time
N = 3;    % Prediction horizon
Duration = 10; % Simulation horizon

radius = 0.01;

global simulation_result;

for m=1:100

    
x0 = 9.5 + 5 * radius*rand(1);
y0 = -4.5 + 5 * radius*rand(1);
z0 = 2.1 + radius*rand(1);
w0 = 1.5 + radius*rand(1);


x = [x0;y0;z0;w0;];

simulation_result = x;


x_now = zeros(4,1);
x_next = zeros(4,1);

x_now = x;

% Start simulation
for ct = 1:(Duration/Ts) 
     u = NN_output(x_now,20,1,'neural_network_controller_2');
     
     
    x_next = system_eq_dis(x_now, Ts, u);

    x = x_next;
    x_now = x_next;
%     x_now
end
plot(simulation_result(1,:),simulation_result(2,:), 'color', 'r');
xlabel('x');
ylabel('y');
hold on;

    
    
end

