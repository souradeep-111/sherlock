function final_val = system_eq_dis(x_initial,time, control_input)

global simulation_result;

function dxdt = ex_13(t,x)
 
    dxdt =[ x(2) ;
            control_input * x(2)^2 - x(1);];

end


[t ,y] = ode45(@ex_13, [0 time], x_initial);

simulation_result = [simulation_result y'];


s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end