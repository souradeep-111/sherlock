function final_val = system_eq_dis(x_initial,time, control_input)
global simulation_result;

function dxdt = system_cont(t,x)
    
    dxdt = [  x(3);
              x(4);
              x(5);
              x(6);
              -2*x(5)-4-0.0001*x(3)^2;
              -2*x(6)+2 * control_input - 0.0001*x(4)^2];

end

[t ,y] = ode45(@system_cont, [0 time],x_initial);


s = size(y);
final_val = y(s(1),:);
final_val = final_val';

simulation_result = [simulation_result final_val];

end