function [m, beta] = ngaussNoiseSites(noise, g, nu, mu, varSigma, y)

% NGAUSSNOISESITES Site updates for noiseless Gaussian noise model.

% IVM

[m, beta] = gaussianNoiseSites(noise, g, nu, mu, varSigma, y);