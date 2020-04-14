function final_val = system_eq_dis(x_initial,time, control_input)

% global simulation_result;

function dxdt = car_model(t,x)
  %vehicleODE Bicycle model of a vehicle with
  % states
  %       x(1), x(2): x,y positions
  %       x(3): Yaw angle (\psi)
  %       x(4): velocity
  % control inputs
  %       u(1): acceleration m/s^2
  %       u(2): steering angle of front wheel
  
      lr = 1.5;
      lf = 1.8;

      disturbance = zeros(1,3);
      dxdt = zeros(4,1);
      
      arg = (lr + disturbance(3))/((lr + disturbance(3)) + (lf + disturbance(2))) * tan(control_input(2));
      beta = arg - arg^3/3 + arg^5/5 - arg^7/7;
%       beta = atan((lr + disturbance(3))/((lr + disturbance(3)) + (lf + disturbance(2))) * tan(control_input(2)));

%       dxdt(1) = x(4) * cos(x(3) + beta);
%       dxdt(2) = x(4) * sin(x(3) + beta);
%       dxdt(3) = x(4)/(lr + disturbance(3)) * sin(beta);
%       dxdt(4) = control_input(1)+disturbance(1);
      dxdt(1) = x(4) * cos(x(3));
      dxdt(2) = x(4) * sin(x(3));
      dxdt(3) = control_input(2);
      dxdt(4) = control_input(1)+disturbance(1);
end

[t ,y] = ode45(@car_model, [0 time],x_initial);



% simulation_result = [simulation_result y'];

s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end