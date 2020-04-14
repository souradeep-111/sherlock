function y = objective_fcn(u,x,Ts,N,x_ref,u_last)

r = 0.01; % Weight on the slew rate

cost = 0;
x_current = x;
Q = zeros(4,4);
Q(1,1) = 2;
Q(2,2) = 2;
Q(3,3) = 1;
Q(4,4) = 1;
for i = 1:N
    
    x_next = system_eq_dis(x_current, Ts,u(i,:));
    
%     state_space_term = norm((x_next - x_ref), 2);
    state_space_term = (x_next - x_ref)' * Q * (x_next - x_ref);

    if( i == 1)
        slew_term = norm( (u_last - u(i,:)) , 2 );
    else
        slew_term = norm( (u(i-1) - u(i,:)) , 2 );
    end
    cost = cost + ( state_space_term + r * slew_term ) ; 
    
    x_current = x_next;
    
end
y = cost;

end