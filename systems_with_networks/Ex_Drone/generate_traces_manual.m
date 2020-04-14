no_of_traces = 50;
no_of_steps = 50;
time_step = 0.1;
rad = 0.05;

interested_variable = 1;

fnameNN = ['NN_sherlock/controller_', num2str(1),'.nt'];

mid_point_init = zeros(12,1);
mid_point_init(1) = -0.0225;
mid_point_init(2) = 0.3089;
mid_point_init(3) = 0.2086;
mid_point_init(4) = 0.1407;
mid_point_init(5) = 0.6676;
mid_point_init(6) = 0.3038;
mid_point_init(7) = 0.0138;
mid_point_init(8) = -0.0110;
mid_point_init(9) = 0.0093;
mid_point_init(10) = -0.0366;
mid_point_init(11) = -0.0002;
mid_point_init(12) = -0.0002;


simulation_count = 0;
while(simulation_count < no_of_traces )


  plot_time = zeros(1, no_of_steps+1);
  plot_variable = zeros(1, no_of_steps+1);

%   initial_state = randomStateInInitialSetDrone(params, path);

%   initial_state
  initial_state = zeros(12,1);
  initial_state(1) = mid_point_init(1) + (rad * rand);
  initial_state(2) = mid_point_init(2) + (rad * rand);
  initial_state(3) = mid_point_init(3) + (rad * rand);
  initial_state(4) = mid_point_init(4) + (rad * rand);
  initial_state(5) = mid_point_init(5) + (rad * rand);
  initial_state(6) = mid_point_init(6) + (rad * rand);
  initial_state(7) = mid_point_init(7) + (rad * rand);
  initial_state(8) = mid_point_init(8) + (rad * rand);
  initial_state(9) = mid_point_init(9) + (rad * rand);
  initial_state(10) = mid_point_init(10) + (rad * rand);
  initial_state(11) = mid_point_init(11) + (rad * rand);
  initial_state(12) = mid_point_init(12) + (rad * rand);

  x0 = initial_state;

  plot_time(1) = x0(interested_variable);
  plot_variable(1) = x0(interested_variable+1);
  i = 1;
  while(i <= no_of_steps)

    u = NN_output(x0, 100, 1, fnameNN);

    
    % ODE integration
    odefun = @(t,x) plant_dynamics(u, t, x );

    [ts, st] = ode45(odefun, 0:(time_step/50):time_step, x0);
    k = size(ts,1);
    x0 = st(k,:)' ;

%     plot_time(i+1) = i * time_step;
    plot_time(i+1) = x0(interested_variable);
    plot_variable(i+1) = x0(interested_variable+1);
    % x0
    i = i + 1;
  end

  % x0
  simulation_count = simulation_count + 1;

  plot(plot_time, plot_variable, 'color', 'r');
  xlabel(['x_', num2str(interested_variable)]);
  ylabel(['x_', num2str(interested_variable + 1)]);
  hold on;
end
