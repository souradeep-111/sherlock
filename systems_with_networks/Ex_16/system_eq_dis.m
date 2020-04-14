function final_val = system_eq_dis(x_initial,time, control_input)

global simulation_result;

function dxdt = ex_16(t,x)
 
    dxdt =[ -x(1) + x(2) - x(3);
            -x(1)*(x(3) + 1) - x(2);
            -x(1) + control_input;];

end


[t ,y] = ode45(@ex_16, [0 time], x_initial);

simulation_result = [simulation_result y'];

s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end