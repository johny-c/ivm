function e = ivmNegLogLikelihood(params, model, prior)

% IVMNEGLOGLIKELIHOOD Wrapper function for calling ivm likelihoods.

% IVM

model.noise = noiseExpandParam(model.noise, params);
e = - ivmLogLikelihood(model);

if prior
  e =e +0.5*params*params';
end