load('controller_3_20.mat');
no_of_hidden_layers = network.number_of_layers - 1;
no_of_inputs = network.number_of_inputs;
no_of_outputs = network.number_of_outputs;
network_configuration = network.layer_sizes;


file = fopen('controller.nt','w');

s =  no_of_inputs;
fprintf(file,'%d\n',s);
s = no_of_outputs;
fprintf(file,'%d\n',s);
s = no_of_hidden_layers;
fprintf(file,'%d\n',s);
for i = 1:no_of_hidden_layers
    s = network_configuration(i);
    fprintf(file,'%d\n',s);
end

for layer_index = 1:(no_of_hidden_layers+1)
    for output_index = 1:network_configuration(layer_index)
        if(layer_index == 1)
            prev_layer_count = no_of_inputs;
        else
            prev_layer_count = network_configuration(layer_index - 1);
        end
        for input_index = 1:prev_layer_count
            s = network.W{layer_index}(output_index, input_index);
            fprintf(file, '%d\n', s);
        end
        s = network.b{layer_index}(output_index);
        fprintf(file, '%d\n', s);
    end
end
fclose(file);