function[h] = do_thresholding(r)
    [size_1, size_2]  = size(r);
    out = zeros(size_1,1);
    for i = 1:size_1
       if(r(i) > 0)
         out(i) = r(i);
       else
          out(i) = 0;
       end
    end
    h = out;
end