Ts = 0.2;  % Sample Time
N = 2;    % Prediction horizon
Duration = 10; % Simulation horizon


% Generating the data for the MPC Controller : 

options = optimoptions('fmincon','Algorithm','sqp','Display','none');
uopt = zeros(N,2);

% Apply the control input constraints
LB = -1*ones(N,2);
UB = 1*ones(N,2);

for i = 1:N
    LB(i,2) = -10;
    UB(i,2) = 10;
end

% Start simulation
fprintf('Generating the MPC data...  It might take a while...\n')


x = zeros(4,1);
xref1 = zeros(4,1);
xref2 = [5;0;0;0];

xref = xref1;

no_of_data_points = 10;

file = fopen('MPC_data','w');

radius = 0.1 ; % dimension
figure;

for i = 1:no_of_data_points
    
    
    x = [10 + 1 *rand(1);
         -5 + 1 *rand(1);
          2 +  1 *rand(1);
         1.5 + 1*rand(1);];  
  
  
    i
    disp('Starting with initial state')
    x
    xHistory = x;
    uHistory = uopt(1,:);
    
    for j = 1:(Duration/Ts)
        j
        % A simple cost function for the MPC has been implemented here 
        % according to the reference input for the plant
        COSTFUN = @(u) objective_fcn(u,x,Ts,N,xref,uopt(1,:));

        % A simple contraint function which limits the angles and distance
        % travelled by the pendulum
        CONSFUN = @(u) constraint_fcn_for_pendulum(u,x,Ts,N);

        % taking the result of the optimization function implemented
        uopt = fmincon(COSTFUN,uopt,[],[],[],[],LB,UB,CONSFUN,options);

        % Take the first optimal control move.    
        z = uopt(1,:);

        xHistory = [xHistory x]; 
        uHistory = [uHistory uopt(1,:)];
       
        for k = 1:4
            s = x(k);
            fprintf(file,'%d\n',s);
        end
        
        s = z(1);
        fprintf(file,'%d\n',s);

        s = z(2);
        fprintf(file,'%d\n',s);

        % Implement first optimal control move and update plant states.
        x = system_eq_dis(x, Ts, uopt(1,:));
        x
        z
        
        if (mod(j,5) == 1)
        x = x + [0.1 *rand(1);
                 0.1 *rand(1);
                 0.1 *rand(1);
                 0.1 *rand(1);];
        end
    end

    plot(xHistory(1,:),xHistory(2,:));
    xlabel('x-pos');
    ylabel('y-pos');
    hold on;
%     abort
end

fprintf('Data generation finished!\n')
fclose(file);
