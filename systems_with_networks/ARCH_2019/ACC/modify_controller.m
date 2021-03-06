% Read the first layer 
name = 'controller.nt';

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

fclose(file);
input = [30; 1.4; 0 ; 0 ; 0];

% Compute the change in bias due to fixed inputs 
change_in_bias = weight_matrix * input;
new_bias = bias_matrix + change_in_bias;


file = fopen(name,'r');
file_data = fscanf(file,'%f');
fclose(file);



file = fopen('modified_controller_1.nt','w');

no_of_inputs = file_data(1);
fprintf(file,'%d\n',no_of_inputs-2);

no_of_outputs = file_data(2);
fprintf(file,'%d\n',no_of_outputs);

no_of_hidden_layers = file_data(3);
fprintf(file,'%d\n',no_of_hidden_layers);

network_structure = zeros(no_of_hidden_layers+1,1);
pointer = 4;
for i = 1:no_of_hidden_layers
    network_structure(i) = file_data(pointer);
    fprintf(file,'%d\n',file_data(pointer));
    pointer = pointer + 1;
end
network_structure(no_of_hidden_layers+1) = no_of_outputs;

x = zeros(no_of_inputs-2, 1);

weight_matrix = zeros(network_structure(1), no_of_inputs-2);
bias_matrix = zeros(network_structure(1),1);

% READING THE INPUT WEIGHT MATRIX
for i = 1:network_structure(1)
    for j = 1:no_of_inputs
        if(j > 2)
            weight_matrix(i,j-2) = file_data(pointer);
            fprintf(file,'%d\n',file_data(pointer));
        end
        pointer = pointer + 1;
    end
    bias_matrix(i) = file_data(pointer);
    fprintf(file,'%d\n',new_bias(i));
    
    pointer = pointer + 1;
end
size(weight_matrix)
size(bias_matrix)
% Doing the input transformation
g = x;
g = weight_matrix * g;
g = g + bias_matrix(:);
g = do_thresholding(g);



for i = 1:(no_of_hidden_layers)
    
    weight_matrix = zeros(network_structure(i+1), network_structure(i));
    bias_matrix = zeros(network_structure(i+1),1);
    size(weight_matrix)
    size(bias_matrix)
    % READING THE WEIGHT MATRIX
    for j = 1:network_structure(i+1)
        for k = 1:network_structure(i)
            weight_matrix(j,k) = file_data(pointer);
            fprintf(file,'%d\n',file_data(pointer));
            
            pointer = pointer + 1;
        end
        bias_matrix(j) = file_data(pointer);
        fprintf(file,'%d\n',file_data(pointer));
        
        pointer = pointer + 1;
    end
   
    % Doing the transformation
    g = weight_matrix * g;
    g = g + bias_matrix(:);
    g = do_thresholding(g);

end
% Some bogus inputs
offset = 0;
scale_factor = 0;
y = g-offset;
y = y * scale_factor;
fclose(file);


