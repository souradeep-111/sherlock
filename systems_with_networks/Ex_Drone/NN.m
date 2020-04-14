function yOut = NN(params, nnParams, xInp)

    nLayers = nnParams.nLayers;
    cur_in = xInp;
    for j = 1:(nLayers-1)
       x1 = nnParams.W{1,j}*cur_in + nnParams.b{1,j};
       cur_in = max(x1, zeros(size(x1,1),1));
    end
    yOut = nnParams.W{1,nLayers}*cur_in + nnParams.b{1,nLayers};
    
    % saturation over control inputs
    yOut = max(params.lbControlInputs, yOut);
    yOut = min(params.ubControlInputs, yOut);

end
