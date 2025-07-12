close all;
clear();

% Parameters
fs = 4000;
N = 128;
M = 160;
f0 = 950; % 0;   
df = 7/fs; % (1-1/M)/M; -> for full scale
w0 = 2 * pi * (f0 / fs);
dw = 2*pi*df;

W = cos(dw) - 1i * sin(dw);

n = 0:1:N + M - 2; 
h1 = W .^ ((-(n-N+1).^2) ./ 2);

n = 0:1:N-1; 
m1 = [(cos(w0 .* n) - 1i * sin(w0 .* n)) .* W .^ ((n .^ 2) ./ 2), zeros(1, N + M - length(n)-1)];

n = N-1:1:N + M - 2; 
m2 = [zeros(1, N-1), W .^ (((n-N+1).^2) ./ 2);];

%%%% ------------------ TEST ------------------ %%%%

Amplitude = (2^8) - 1;

f1 = 1500;
f2 = 1700;

dt = 1 / fs;                                     
t = 0 : dt : N * dt - dt;
x = Amplitude * sin(2 * pi * f1 * t) + Amplitude * sin(2 * pi * f2 * t);
dc = sum((x)) / length(x);

x = x-dc;
pw = sum(abs(x).^2) / length(x);

x = [x-dc, zeros(1, length(h1) - length(x))];

% using FFT
mix_up_fft = x .* m1;

X_fft = fft(mix_up_fft);
H_fft = fft(h1);
Y_fft = X_fft .* H_fft;
mix_down_fft = ifft(Y_fft);

y_fft = mix_down_fft .* m2;

% using FIR
mix_up_fir = x .* m1;

%mix_down_fir = filter(h1 ,1 , mix_up_fir);
x_re = real(mix_up_fir);
x_im = imag(mix_up_fir);
h1_re = real(h1);
h1_im = imag(h1);
mix_down_fir_ReRe = filter(h1_re, 1, x_re);
mix_down_fir_ImRe = filter(h1_re, 1, x_im);
mix_down_fir_ReIm = filter(h1_im, 1, x_re);
mix_down_fir_ImIm = filter(h1_im, 1, x_im);

%combine
mix_down_fir = mix_down_fir_ReRe - mix_down_fir_ImIm + 1i * (mix_down_fir_ReIm + mix_down_fir_ImRe);

y_fir = mix_down_fir .* m2;

% FFT for comparison
X = abs(fft(x, fs));
f_X = (0:length(X)-1) * fs / length(X);

y_fir = abs(y_fir(N:end));
y_fft = abs(y_fft(N:end));
f_y = f0 + (0 : length(y_fft)-1) .* df * fs;

figure();
hold on;
plot(f_X, X);
plot(f_y, y_fft);
plot(f_y, y_fir);
title("Chirp Transform Algorithm");
ylabel('Amplitude');
xlabel('Frequency in Hz');
legend('FFT of signal', 'CTA FFT of signal', 'CTA FIR of signal');
grid on;



