function final_val = system_eq_dis(x_initial,time, control_input)

global simulation_result;

function dxdt = ex_14(t,x)
 
    dxdt =[ -x(1)*(0.1 + (x(1) + x(2))^2) ;
            (control_input + x(1)) * (0.1 + (x(1) + x(2))^2);];

end


[t ,y] = ode45(@ex_14, [0 time], x_initial);

simulation_result = [simulation_result y'];

s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end