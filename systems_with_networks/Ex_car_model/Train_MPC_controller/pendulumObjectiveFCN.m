function y = objective_fcn(u,x,Ts,N,x_ref,u_last)

r = 0.1; % Weight on the slew rate

cost = 0;
x_current = x;

for i = 1:N
    
    x_next = system_eq_dis(x_current, Ts,u(i));
    
    state_space_term = norm((x_next - x_ref), 2);
    if( i == 1)
        slew_term = norm( (u_last - u(i)) , 2 );
    else
        slew_term = norm( (u(i-1) - u(i)) , 2 );
    end
    cost = cost + ( (1-r) * state_space_term + r * slew_term ) ; 
end

y = cost;

end