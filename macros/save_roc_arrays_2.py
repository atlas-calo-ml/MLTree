#! /usr/bin/env python

import h5py
import numpy as np
from keras.models import load_model
import pdb

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

X_EMB2 = np.load('array_EMB2.npy')
y = np.load('array_y.npy')
nsamples = len(X_EMB2)
train_size = 600000
test_size = nsamples - train_size

from sklearn.cross_validation import train_test_split
X_EMB2_train, X_EMB2_test, y_train, y_test, = train_test_split(X_EMB2, y, test_size=test_size, random_state=1)

model_EMB2_conv = load_model('keras_convolutional_20161215_0_EMB2_64_6_6666000_train600000_test66000.h5')
p_EMB2_conv = model_EMB2_conv.predict(X_EMB2_test)
# pdb.set_trace()

array = np.arange(0, 1, 0.1)

pi0_eff_EMB2_conv = np.zeros((len(array),1))
piplus_eff_EMB2_conv = np.zeros((len(array),1))

for i,cut in enumerate(array):
  print('i', i)

  x = p_EMB2_conv[:,0]<cut
  pi0_eff_EMB2_conv[i] = sum(np.all([x, (1-y_test)], axis=0))/float(sum(y_test == 0))
  piplus_eff_EMB2_conv[i]  = sum(np.all([1-x, (y_test)], axis=0))/float(sum(y_test == 0))

print ('----> Saving arrays to file')
np.save('roc_pi0_eff_EMB2_conv.npy', pi0_eff_EMB2_conv)
np.save('roc_piplus_eff_EMB2_conv.npy', piplus_eff_EMB2_conv)

# # pdb.set_trace()
# lw=2
# fig = plt.figure(figsize=(8,8))
# plt.plot(pi0_eff_EMB2_conv, piplus_eff_EMB2_conv, lw=lw, linestyle='-', label='EMB2, 1 layer NN with 128 neurons')
# plt.plot(pi0_eff_EMB2_conv_64_32, piplus_eff_EMB2_conv_64_32, lw=lw, linestyle='-', label='EMB2, 3 layered NN with 128,64,32 neurons')
# plt.plot([0, 1], [1, 0], color='gray', lw=lw, linestyle='--', label='Random')
# plt.xlim([0.0, 1.0])
# plt.ylim([0.0, 1.0])
# plt.title('ROC curve')
# plt.xlabel('$\pi^0$ efficiency')
# plt.ylabel('$\pi^{\pm}$ efficiency')
# plt.legend(loc="lower left")
# fig.savefig('roc_test.pdf', bbox_inches='tight')
