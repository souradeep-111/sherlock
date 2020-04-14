clear all;
Ts = 0.1;  % Sample Time
Duration = 5; % Simulation horizon

global simulation_result;

min = 1e10;
for m=1:100

    
x1 = 90 + 1.0*rand(1);
x2 = 10 + 1.0*rand(1);
x3 = 30.0 + 0.2*rand(1);
x4 = 30.0 + 0.2*rand(1);
x5 = 0 + 0.01*rand(1);
x6 = 0 + 0.01*rand(1);


x = [x1;x2;x3;x4;x5;x6];

simulation_result = x;


% Things specfic to the model  ends here


u_max = 0;


x_now = zeros(6,1);
x_next = zeros(6,1);
time = zeros(Duration/Ts+1, 1);

x_now = x;

% Start simulation
for ct = 1:(Duration/Ts)

%     network_input = [30; 1.4; x_now(4);x_now(1) - x_now(2);x_now(3)-x_now(4)];
%     u = NN_output(network_input, 10, 1,'controller.nt');
%     network_input = [ x_now(4);x_now(1) - x_now(2);x_now(3)-x_now(4)];
%     u = NN_output(network_input, 10, 1,'modified_controller_1.nt');
%     
    network_input = [ x_now(1);x_now(2);x_now(3);x_now(4);x_now(5);x_now(6)];
    u = NN_output(network_input, 10, 1,'modified_controller_2.nt');
    
    x_next = system_eq_dis(x_now, Ts, u);

    x = x_next;
    x_now = x_next;

    time(ct+1) = ct*Ts;
    
    if(u < min)
        min = u;
    end
end

% plot(simulation_result(1,:),simulation_result(2,:), 'color', 'r');
% xlabel('x_1');
% ylabel('x_2');
% hold on;
hold on;
plot(simulation_result(3,:),simulation_result(4,:), 'color', 'r');
xlabel('v\_1');
ylabel('v\_2');


% subplot(2,1,1);
% plot(time',simulation_result(3,:));
% xlabel('time');
% ylabel('v_1');
% hold on;
% 
% subplot(2,1,2);
% plot(time',simulation_result(4,:));
% xlabel('time');
% ylabel('v_2');
%  hold on;   
    
end
min