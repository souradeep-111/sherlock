function final_val = system_eq_dis(x_initial,time, control_input)

global simulation_result;

function dxdt = ball_and_beam(t,x)
 
    dxdt =[ x(2);
            -9.8*x(3) + 1.6 * x(3)^3 + x(1)*x(4)^2;
            x(4);
            control_input;];

end

[t ,y] = ode45(@ball_and_beam, [0 time],x_initial);

simulation_result = [simulation_result y'];

s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end