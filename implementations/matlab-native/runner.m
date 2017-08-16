function runner(twoExp)
% 'twoExp' is the exponent of two that determines the size of the array
% Example: runner(10)

n = power(2,twoExp);
if or(twoExp < 0, twoExp > 30)
    error('ERROR: invalid exponent of %d for input size\n',n);
    exit(1);
end

m = repmat(sinComplexVector(n,1,1),n,1);

tic;
res = fft2(m);
elapsedTime = toc;

nb_significant_signals = sum(sum(res>10));
amplitude = res(1,3);
fprintf('nb of significant signals: %d\n', nb_significant_signals);
fprintf('amplitude at (1,3): %f\n', amplitude);

e = log10(real(amplitude));

if (e > 0)
    e = ceil(e);
else
    e = floor(e);
end

normalized_amplitude = floor((1000/(10^e))*amplitude);

fprintf('{ \"status\": %d, \"options\": \"%d\", \"time\": %f, \"output\": { \"nb-significant-signals\": %d, \"normalized-amplitude\": %d } }\n', 1, twoExp, elapsedTime, nb_significant_signals, normalized_amplitude);
end
