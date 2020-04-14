function final_val = system_eq_dis(x_initial,time, control_input)

global simulation_result;

function dxdt = tora(t,x)
 
    e = 0.1;
    dxdt =[ x(2);
            -x(1) + e * sin(x(3)) ;
            x(4);
            control_input ;];
end

[t ,y] = ode45(@tora, [0 time],x_initial);

simulation_result = [simulation_result y'];

s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end