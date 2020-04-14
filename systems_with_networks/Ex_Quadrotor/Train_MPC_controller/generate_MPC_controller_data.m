Ts = 1;  % Sample Time
N = 2;    % Prediction horizon
Duration = 20; % Simulation horizon


% Generating the data for the MPC Controller : 

options = optimoptions('fmincon','Algorithm','sqp','Display','none');
uopt = zeros(N,1);

% Apply the control input constraints
LB = -80*ones(N,1);
UB = 80*ones(N,1);

% Start simulation
fprintf('Generating the MPC data...  It might take a while...\n')


x = zeros(18,1);
xref1 = zeros(18,1);
xref2 = [5;0;0;0];

xref = xref1;

no_of_data_points = 50;

file = fopen('MPC_data','w');

% s = no_of_data_points^4; % no of samples
% fprintf(file,'%d\n',s);

radius = 2 ; % dimension

for i = 1:no_of_data_points
    
    
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
  
  
    i
    disp('Starting with initial state')
    x
    
    for j = 1:(Duration/Ts)
        j
        % A simple cost function for the MPC has been implemented here 
        % according to the reference input for the plant
        COSTFUN = @(u) objective_fcn(u,x,Ts,N,xref,uopt(1));

        % A simple contraint function which limits the angles and distance
        % travelled by the pendulum
        CONSFUN = @(u) constraint_fcn_for_pendulum(u,x,Ts,N);

        % taking the result of the optimization function implemented
        uopt = fmincon(COSTFUN,uopt,[],[],[],[],LB,UB,CONSFUN,options);

        % Take the first optimal control move.    
        z = uopt(1);

       
        for k = 1:18
            s = x(k);
            fprintf(file,'%d\n',s);
        end
        
        s = z;
        fprintf(file,'%d\n',s);

        % Implement first optimal control move and update plant states.
        x = system_eq_dis(x, Ts, uopt(1));
   
    end
end

fprintf('Data generation finished!\n')
fclose(file);
