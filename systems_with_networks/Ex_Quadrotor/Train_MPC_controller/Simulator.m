Ts = 1;  % Sample Time
N = 2;    % Prediction horizon
Duration = 20; % Simulation horizon

% For a usual control 0.1 , 5, 20

% Things specfic to the model starts here 

% Setting the initial state
% x = [-1;-1;9;-1;-1;-1;0;0;0;1;-1;-1;-1;0;0;0;0;0;];
radius = 1;
x = [-1 + radius*rand(1);
    -1 + radius*rand(1);
     9 + radius*rand(1);
     -1 + radius*rand(1);
     -1 + radius*rand(1);
     -1 + radius*rand(1);
      0;
      0;
      0;
      1;
      -1 + radius*rand(1);
      -1 + radius*rand(1);
      -1 + radius*rand(1);
      0;
      0;
      0;
      0;
      0;];  
xref1 = zeros(18,1);

file = fopen('MPC_data','a');


% Things specfic to the model  ends here

options = optimoptions('fmincon','Algorithm','sqp','Display','none');
uopt = zeros(N,1);

u_max = 0;

% Apply the control input constraints
% LB = -300*ones(N,1);
% UB = 300*ones(N,1);
LB = -80*ones(N,1);
UB = 80*ones(N,1);

% Start simulation
fprintf('Simulation started.  It might take a while...\n')
xHistory = x;
uHistory = uopt(1);
for ct = 1:(Duration/Ts)
    ct
    % Set references.
%     if ct*Ts<10
%         xref = xref1;
%     else
%         xref = xref2;
%     end

    xref = xref1;
    
    % A simple cost function for the MPC has been implemented here 
    % according to the reference input for the plant
    COSTFUN = @(u) objective_fcn(u,x,Ts,N,xref,uopt(1));
    
    % A simple contraint function which limits the angles and distance
    % travelled by the pendulum
    CONSFUN = @(u) constraint_fcn_for_pendulum(u,x,Ts,N);
    
    % taking the result of the optimization function implemented
    uopt = fmincon(COSTFUN,uopt,[],[],[],[],LB,UB,CONSFUN,options);
%     uopt
    % Implement first optimal control move and update plant states.
    x = system_eq_dis(x, Ts, uopt(1));
    
% Save plant states for display.
    xHistory = [xHistory x]; 
    uHistory = [uHistory uopt(1)];

%     Printing stuff

%     disp('Height = ');
    x
    disp('Control input = ');
    uopt(1)
    
%     disp('Network controller = ');
%     neural_network_controller(x)

    
end

fclose(file);

fprintf('Simulation finished!\n')

figure;

plot(0:Ts:Duration,xHistory(3,:));
xlabel('time');
ylabel('Height');
