Ts = 0.5;  % Sample Time
N = 3;
Duration = 10; % Simulation horizon

global simulation_result;

radius = 0.1;

for m=1:1000

    
x0 = 0.35 + radius*rand(1);
y0 = 0.45 + radius*rand(1);
z0 = 0.25 + radius*rand(1);

x = [x0;y0;z0;];

simulation_result = x;

options = optimoptions('fmincon','Algorithm','sqp','Display','none');
uopt = zeros(N,1);

u_max = 0;

% Apply the control input constraints
LB = -3*ones(N,1);
UB = 3*ones(N,1);

x_now  = zeros(3,1);
x_next = zeros(3,1);
z = zeros(4,1);


% Start simulation
for ct = 1:(Duration/Ts)
    uopt(1) = NN_output(x, 100, 0.1, 'modified_controller');
    
    x = system_eq_dis(x, Ts, uopt(1));
    
end
plot(simulation_result(1,:),simulation_result(2,:), 'color', 'r');
xlabel('x');
ylabel('y');
hold on;

    
    
end