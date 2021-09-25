# Script converts analog filter transfer function to digital domain
# and calculates coefficients for biquad cascade IIR digital filter.

clear all
close all

Fs = 40000;

# Definition of analog A-weighting filter according to IEC/CD 1672.
f1 = 20.598997; 
f2 = 107.65265;
f3 = 737.86223;
f4 = 12194.217;
a1000 = 1.9997;

num = [(2*pi*f4)^2*(10^(a1000/20)) 0 0 0 0];
den = conv([1 +4*pi*f4 (2*pi*f4)^2],[1 +4*pi*f1 (2*pi*f1)^2]); 
den = conv(conv(den,[1 2*pi*f3]),[1 2*pi*f2]);

# Definition of analog C-weighting filter according to IEC/CD 1672.
%f1 = 20.598997; 
%f4 = 12194.217;
%c1000 = 0.0619;
%
%num = [(2*pi*f4)^2*(10^(c1000/20)) 0 0];
%den = conv([1 +4*pi*f4 (2*pi*f4)^2],[1 +4*pi*f1 (2*pi*f1)^2]); 

# Use bilinear to transform it to a discrete-time IIR filter.
[numz,denz] = bilinear(num,den,1/Fs);

# Visualize filters frequency response
figure
fmin = 10;
fmax = Fs + 0.1*Fs;
omega = logspace(log10(2*pi*fmin), log10(2*pi*fmax));

# Analog filter
hs = freqs(num,den,omega);
semilogx(omega/(2*pi), 20*log10(abs(hs)), 'color', 'b', 'linewidth', 2);
grid on; hold on;

# Digital filter
n = ceil(Fs/fmin);
[hz,wz] = freqz(numz,denz,n);
semilogx(wz*Fs/(2*pi), 20*log10(abs(hz)), 'color', 'r', 'linewidth', 2, 'linestyle', '--');

ylim([-100 10])
legend('analog', 'digital')
xlabel('Frequency (Hz)')
ylabel('Magnitude (dB)')

# Get digital filter coefficients (for biquad cascade IIR)
sos = tf2sos(numz, denz)

