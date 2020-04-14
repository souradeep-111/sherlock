Ts = 0.1;  % Sample Time
Duration = 5; % Simulation horizon

x = [0;0;pi;0];

% Start simulation
fprintf('Simulation started.  It might take a while...\n')
xHistory = x;
for ct = 1:(Duration/Ts)
    ct
    u = neural_network_controller(x);
    x = system_eq_dis(x, Ts, u);
    xHistory = [xHistory x]; 
%     Printing stuff

    disp('Angle = ');
    x(3)
    disp('Position = ');
    x(1)

end

fprintf('Simulation finished!\n')

figure;
subplot(2,2,1);
plot(0:Ts:Duration,xHistory(1,:));
xlabel('time');
ylabel('z');
title('cart position');
subplot(2,2,2);
plot(0:Ts:Duration,xHistory(2,:));
xlabel('time');
ylabel('zdot');
title('cart velocity');
subplot(2,2,3);
plot(0:Ts:Duration,xHistory(3,:));
xlabel('time');
ylabel('theta');
title('pendulum angle');
subplot(2,2,4);
plot(0:Ts:Duration,xHistory(4,:));
xlabel('time');
ylabel('thetadot');
title('pendulum velocity');