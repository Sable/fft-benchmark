function [v] = sinComplexVector(n,amplitude,period_divisor)

v = complex(amplitude*sin(linspace(0,period_divisor*4*pi,n)));

end

