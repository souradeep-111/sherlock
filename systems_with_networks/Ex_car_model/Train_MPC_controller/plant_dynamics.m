function dx = plant_dynamics(u, ~,  x)
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
      dx = zeros(4,1);
      
      arg = (lr + disturbance(3))/((lr + disturbance(3)) + (lf + disturbance(2))) * tan(u(2));
%       beta = arg - arg^3/3 + arg^5/5 - arg^7/7;
      beta = atan((lr + disturbance(3))/((lr + disturbance(3)) + (lf + disturbance(2))) * tan(u(2)));

      dx(1) = x(4) * cos(x(3) + beta);
      dx(2) = x(4) * sin(x(3) + beta);
      dx(3) = x(4)/(lr + disturbance(3)) * sin(beta);
      dx(4) = u(1)+disturbance(1);
end
