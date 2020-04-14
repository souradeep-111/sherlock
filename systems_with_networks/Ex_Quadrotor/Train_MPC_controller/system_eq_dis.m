function final_val = system_eq_dis(x_initial,time, control_input)

global simulation_result;

function dxdt = quadrotor(t,x)
  pr = 0;
  qr = 0;
  hr = 0;
  rr = 0;
  
  pn = x(1);
  pe = x(2);
  h = x(3);
  u = x(4);
  v = x(5);
  w = x(6);
  q0 = x(7);
  q1 = x(8);
  q2 = x(9);
  q3 = x(10);
  p = x(11);
  q = x(12);
  r = x(13);
  pI = x(14);
  qI = x(15);
  rI = x(16);
  hI = x(17);
  
  pn_ = 2*u*(q0^2 + q1^2 - 0.5) - 2*v*(q0*q3 - q1*q2) + 2*w*(q0*q2 + q1*q3);
  pe_ = 2*v*(q0^2 + q2^2 - 0.5) + 2*u*(q0*q3 + q1*q2) - 2*w*(q0*q1 - q2*q3);
  h_ = 2*w*(q0^2 + q3^2 - 0.5) - 2*u*(q0*q2 - q1*q3) + 2*v*(q0*q1 + q2*q3);
  u_ = r*v - q*w - 11.62*(q0*q2 - q1*q3);
  v_ = p*w - r*u + 11.62*(q0*q1 + q2*q3);
  w_ = q*u - p*v + control_input + 11.62*(q0^2 + q3^2 - 0.5);  
  q0_ = -0.5*q1*p - 0.5*q2*q - 0.5*q3*r;
  q1_ = 0.5*q0*p - 0.5*q3*q + 0.5*q2*r;
  q2_ = 0.5*q3*p + 0.5*q0*q - 0.5*q1*r;
  q3_ = 0.5*q1*q - 0.5*q2*p + 0.5*q0*r;
  
  p_ = (-40.00063258437631*pI - 2.8283979829540325*p) - 1.133407423682400*q*r;
  q_ = (-39.99980452524146*qI - 2.8283752541008109*q) + 1.132078179613602*p*r;
  r_ = (-39.99978909742505*rI - 2.8284134223281210*r) - 0.004695219977601*p*q;
  
  pI_ = p - pr;
  qI_ = q - qr;
  rI_ = r - rr;
  hI_ = h - hr;
  
    dxdt =[ pn_;pe_;h_;u_;v_;w_;q0_;q1_;q2_;q3_;p_;q_;r_;pI_;qI_;rI_;hI_;1];
end

[t ,y] = ode45(@quadrotor, [0 time],x_initial);

simulation_result = [simulation_result y'];

s = size(y);
final_val = y(s(1),:);
final_val = final_val';

end