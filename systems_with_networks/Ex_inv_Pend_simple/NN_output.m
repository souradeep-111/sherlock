function [y] =  NN_output(x,offset,scale_factor,name)


file = fopen(name,'r');
file_data = fscanf(file,'%f');
no_of_inputs = file_data(1);
no_of_outputs = file_data(2);
no_of_hidden_layers = file_data(3);
network_structure = zeros(no_of_hidden_layers+1,1);
pointer = 4;
for i = 1:no_of_hidden_layers
    network_structure(i) = file_data(pointer);
    pointer = pointer + 1;
end
network_structure(no_of_hidden_layers+1) = no_of_outputs;


weight_matrix = zeros(network_structure(1), no_of_inputs);
bias_matrix = zeros(network_structure(1),1);

% READING THE INPUT WEIGHT MATRIX
for i = 1:network_structure(1)
    for j = 1:no_of_inputs
        weight_matrix(i,j) = file_data(pointer);
        pointer = pointer + 1;
    end
    bias_matrix(i) = file_data(pointer);
    pointer = pointer + 1;
end

% Doing the input transformation
g = zeros(no_of_inputs,1);
g = x;
g = weight_matrix * g;
g = g + bias_matrix(:);
g = do_thresholding(g);


for i = 1:(no_of_hidden_layers)
    
    weight_matrix = zeros(network_structure(i+1), network_structure(i));
    bias_matrix = zeros(network_structure(i+1),1);

    % READING THE WEIGHT MATRIX
    for j = 1:network_structure(i+1)
        for k = 1:network_structure(i)
            weight_matrix(j,k) = file_data(pointer);
            pointer = pointer + 1;
        end
        bias_matrix(j) = file_data(pointer);
        pointer = pointer + 1;
    end
   
    % Doing the transformation
    g = weight_matrix * g;
    g = g + bias_matrix(:);
    g = do_thresholding(g);

end

y = g-offset;
y = y * scale_factor;
fclose(file);

end