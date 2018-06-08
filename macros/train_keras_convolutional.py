#! /usr/bin/env python

import h5py
import numpy as np
import keras.backend as K
from keras.layers import Input, Dense, Activation, Dropout, Flatten
from keras.layers.convolutional import Convolution2D
from keras.layers.pooling import MaxPooling2D
from keras.models import Model, Sequential
import pdb

print ('----> Load data')

X = np.load('array_EMB2.npy')
y = np.load('array_Y.npy')
nsamples = len(X)
train_size = 600000
test_size = nsamples - train_size
tag = '20161215_0_EMB2_64_6_6{:d}_train{:d}_test{:d}'.format(nsamples, train_size, test_size)
print ('X.shape:', X.shape)
print ('y.shape:', y.shape)
print ('Sample size:', nsamples)
print ('Training size:', train_size)
print ('Test size:', test_size)

print ('---->  Split into training and test sets')

from sklearn.cross_validation import train_test_split
X_train, X_test, y_train, y_test, = train_test_split(X, y, test_size=test_size, random_state=1)

print ('---->  Running training')

model_s2 = Sequential()
model_s2.add(Convolution2D(
    64, 6, 6, border_mode='same',
    input_shape=X[0].shape))
model_s2.add(Activation('relu'))
model_s2.add(MaxPooling2D((2, 2), dim_ordering='th'))
model_s2.add(Dropout(0.2))
model_s2.add(Flatten())
model_s2.add(Dense(128))
model_s2.add(Activation('relu'))
model_s2.add(Dropout(0.2))
model_s2.add(Dense(16))
model_s2.add(Activation('relu'))
model_s2.add(Dense(1))
model_s2.add(Activation('sigmoid'))

model_s2.compile(
  optimizer='rmsprop',
  loss='binary_crossentropy',
  metrics=['accuracy'])

model_s2.fit(
  X_train,
  y_train,
  nb_epoch=20,
  batch_size=128,
  validation_data=(X_test, y_test),
  # callbacks=[
  #   EarlyStopping(verbose=True, patience=5, monitor='val_loss'),
  #   ModelCheckpoint(
  #     filename, monitor='val_loss',
  #     verbose=True, save_best_only=True)]
  )

p = model_s2.predict(X_test)
print p
# pdb.set_trace()
print ('p>0.5==y_test: {:.2f}'.format(sum((p[:,0]>0.5)==y_test)/float(len(p))))

model_s2.save('keras_convolutional_'+tag+'.h5')
