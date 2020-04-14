function [y] =  neural_network_controller(x)


file = fopen('neural_network_information','r');
file_data = fscanf(file,'%f');
no_of_inputs = file_data(1);
no_of_outputs = file_data(2);
no_of_hidden_layers = file_data(3);
no_of_neurons = file_data(4);
pointer = 5;

inner_weight_matrix  = zeros(no_of_hidden_layers - 1, no_of_neurons, no_of_neurons);

inner_bias_matrix = zeros(no_of_hidden_layers,no_of_neurons);
output_bias_matrix = zeros(no_of_outputs,1);

input_weight_matrix = zeros(no_of_neurons, no_of_inputs);
output_weight_matrix = zeros(no_of_outputs, no_of_neurons);



% READING THE INPUT WEIGHT MATRIX
for i = 1:no_of_neurons
    for j = 1:no_of_inputs
        input_weight_matrix(i,j) = file_data(pointer);
        pointer = pointer + 1;
    end
    inner_bias_matrix(1,i) = file_data(pointer);
    pointer = pointer + 1;
end

% % READING THE INNER WEIGHT MATRICES FOR ALL THE INNER LAYERS
% 
% for i = 2:no_of_hidden_layers
%     for j = 1:no_of_neurons
%         for k = 1:no_of_neurons
%             inner_weight_matrix(i-1,j,k) = file_data(pointer);
%             pointer = pointer + 1;
%         end
%         inner_bias_matrix(i,j) = file_data(pointer);
%         pointer = pointer + 1;
%     end
% end

% READING THE OUTPUT WEIGHT MATRIX
for i = 1:no_of_outputs
    for j = 1:no_of_neurons
        output_weight_matrix(i,j) = file_data(pointer);
        pointer = pointer + 1;
    end
    output_bias_matrix(i,1) = file_data(pointer);
    pointer = pointer + 1;
end


g = zeros(no_of_inputs,1);
g = x;
g = input_weight_matrix * g;
g = g + inner_bias_matrix(1,:)';

g = do_thresholding(g);


% for i = 2: no_of_hidden_layers
%     for j = 1: no_of_neurons
%         for k = 1: no_of_neurons
%             A(j,k) = inner_weight_matrix(i-1,j,k);
%         end
%     end
%     g = A * g;
%     g = g + inner_bias_matrix(i,:)';
%     g = do_thresholding(g);
% end

g = output_weight_matrix * g;
g = g + output_bias_matrix(1,1);
g = do_thresholding(g);

y = g-80;

end