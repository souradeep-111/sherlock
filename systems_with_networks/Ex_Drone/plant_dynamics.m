function dx = plant_dynamics(u, ~, x) % params unuseful ???

% Crazyflie 2.0 quadrotor model with
% states
%       x(1), x(2), x(3): x, y, z positions
%       x(4), x(5), x(6): dotx, doty, dotz derivatives of positions
%       x(7), x(8), x(9): phi, theta, psi (roll, pitch and yaw angles)
%       x(10), x(11), x(12): dotphi, dottheta, dotpsi derivatives of angles
% control inputs
%       u(1): Thrust
%       u(2), u(3), u(4): Roll, pitch and yaw torque


    % attitude controller (continuous-time proportional controllers)
    Kphi = 1;%6;
    Ktheta = 1;%6;
    Kpsi = 1;
    KdotPhi = 2.79e-5;
    KdotTheta = 2.8e-5;
    KdotPsi = 4.35e-5;
    g = 9.81; % m.s^(-2)
    m = 3.3e-2; % kg
    Ix = 1.395e-5; % kg.m^2
    Iy = 1.436e-5; % kg.m^2
    Iz = 2.173e-5; % kg.m^2

    PhiCommand = Kphi*(u(2)-x(7));
    ThetaCommand = Ktheta*(u(3)-x(8));
    PsiCommand = Kpsi*(u(4)-x(9));
    % attitude rates controller (continuous-time proportional controllers)
    dotPhiCommand = KdotPhi*(PhiCommand-x(10));
    dotThetaCommand = KdotTheta*(ThetaCommand-x(11));
    dotPsiCommand = KdotPsi*(PsiCommand-x(12));

    % quadrotor dynamics
    dx = zeros(12,1);

    dx(1) = x(4);
    dx(2) = x(5);
    dx(3) = x(6);
    dx(4) = x(8)*u(1)/m;
    dx(5) = -x(7)*u(1)/m;
    dx(6) = (-m*g + u(1))/m;
    dx(7) = x(10);
    dx(8) = x(11);
    dx(9) = x(12);
    dx(10) = dotPhiCommand/Ix;
    dx(11) = dotThetaCommand/Iy;
    dx(12) = dotPsiCommand/Iz;

end
