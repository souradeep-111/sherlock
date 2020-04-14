% params = setParams();
% path = generateDesiredPath(params);
% way_points = path.wayPoints;

no_of_traces =10;
no_of_steps = 50;
time_step = 0.2;
interested_variable = 1;

% fnameNN = ['NN/', num2str(1) ,'/nn1', '/nn.mat'];
% nnParams = loadNN(params.nControlInputs,fnameNN);


radius = 0.01;

simulation_count = 0;
while(simulation_count < no_of_traces )


  plot_time = zeros(1, no_of_steps+1);
  plot_variable = zeros(1, no_of_steps+1);

%   x0 = [10.5 + radius*rand(1);
%          -4.5 + radius*rand(1);
%           1.5 + radius*rand(1);
%          2 +  radius*rand(1);];  

    x0 = [9.0 + 50 * radius*rand(1);
         -4.5 + 50 * radius*rand(1);
          2.1 + radius*rand(1);
         1.5 +  radius*rand(1);]; 
     
  plot_time(1) = x0(interested_variable);
  plot_variable(1) = x0(interested_variable+1);
  i = 1;
  while(i <= no_of_steps)
    u = NN_output(x0, 20, 1, 'neural_network_controller_2');
    x0
    u
    x0 = system_eq_dis(x0, time_step, u);
    % plot_time(i+1) = i * params.timeStepNN;
    plot_time(i+1) = x0(interested_variable);
    plot_variable(i+1) = x0(interested_variable+1);
    i = i + 1;
  end

  x0
  simulation_count = simulation_count + 1;

  plot(plot_time, plot_variable, 'color', 'r');
  xlabel('time');
  ylabel('x_');
  hold on;
end
