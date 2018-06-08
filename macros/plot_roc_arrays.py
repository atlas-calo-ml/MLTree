#! /usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.pylab as pylab
import seaborn as sns

sns.set_style("whitegrid")
params = {'legend.fontsize': 'large',
      'figure.figsize': (8, 8),
      'axes.labelsize': 'x-large',
      'axes.titlesize':'large',
      'xtick.labelsize':'large',
      'ytick.labelsize':'large'}
pylab.rcParams.update(params)

pi0_eff_EMB2_128 = np.load('roc_pi0_eff_EMB2_128.npy')
piplus_eff_EMB2_128 = np.load('roc_piplus_eff_EMB2_128.npy')
pi0_eff_EMB2_128_64_32 = np.load('roc_pi0_eff_EMB2_128_64_32.npy')
piplus_eff_EMB2_128_64_32 = np.load('roc_piplus_eff_EMB2_128_64_32.npy')
pi0_eff_EMB2_conv = np.load('roc_pi0_eff_EMB2_conv.npy')
piplus_eff_EMB2_conv = np.load('roc_piplus_eff_EMB2_conv.npy')
pi0_eff_allpixels_128_64_32 = np.load('roc_pi0_eff_allpixels_128_64_32.npy')
piplus_eff_allpixels_128_64_32 = np.load('roc_piplus_eff_allpixels_128_64_32.npy')

# # pdb.set_trace()
lw=2
fig = plt.figure(figsize=(8,8))
plt.plot(pi0_eff_EMB2_128, piplus_eff_EMB2_128, lw=lw, linestyle='-', label='EMB2 (flat), 1L NN with 128 neurons')
plt.plot(pi0_eff_EMB2_128_64_32, piplus_eff_EMB2_128_64_32, lw=lw, linestyle='-', label='EMB2 (flat), 3L NN with 128,64,32 neurons')
plt.plot(pi0_eff_EMB2_conv, piplus_eff_EMB2_conv, lw=lw, linestyle='-', label='EMB2 (2D image), Convolutional NN')
plt.plot(pi0_eff_allpixels_128_64_32, piplus_eff_allpixels_128_64_32, lw=lw, linestyle='-', label='All pixels (flat), 3L NN with 128,64,32 neurons')
plt.plot([0, 1], [1, 0], color='gray', lw=lw, linestyle='--', label='Random')
plt.xlim([0.0, 1.0])
plt.ylim([0.0, 1.0])
plt.title('ROC curve')
plt.xlabel('$\pi^0$ efficiency')
plt.ylabel('$\pi^{\pm}$ efficiency')
plt.legend(loc="lower left")
fig.savefig('roc_curve_NN.pdf', bbox_inches='tight')
