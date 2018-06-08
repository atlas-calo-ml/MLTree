#! /usr/bin/env python

import h5py
import numpy as np
import keras.backend as K
from keras.layers import Input, Dense
from keras.models import Model
import pdb

print ('----> Load data')

# X = np.load('array_all_pixels.npy')[:,:,0]
X = np.load('array_EMB2_flat.npy')[:,:,0]
y = np.load('array_y.npy')
# y = np.load('array_random_y.npy') # sanity check with random labels
nsamples = len(X)
train_size = 600000
test_size = nsamples - train_size
# tag = '20161215_0_EMB2_layers_128_64_32_samples{:d}_train{:d}_test{:d}'.format(nsamples, train_size, test_size)
# tag = '20161215_0_allpixels_layers_128_64_32_samples{:d}_train{:d}_test{:d}'.format(nsamples, train_size, test_size)
tag = '20161215_0_EMB2_randomy_layers_128_samples{:d}_train{:d}_test{:d}'.format(nsamples, train_size, test_size)
print ('X.shape:', X.shape)
print ('y.shape:', y.shape)
print ('Sample size:', nsamples)
print ('Training size:', train_size)
print ('Test size:', test_size)

print ('---->  Split into training and test sets')

from sklearn.cross_validation import train_test_split
X_train, X_test, y_train, y_test, = train_test_split(X, y, test_size=test_size, random_state=1)

print ('---->  Running training')

# Look up: functional interface?
inputs = Input(shape=(X.shape[1],))
f = Dense(128, activation="tanh")(inputs)
# f = Dense(64, activation="tanh")(f)
# f = Dense(32, activation="tanh")(f)
f = Dense(1, activation="sigmoid")(f)
model = Model(input=[inputs], output=[f])

model.compile(
  optimizer='rmsprop',
  loss='binary_crossentropy',
  metrics=['accuracy'])

model.fit(
  X_train,
  y_train,
  nb_epoch=50,
  batch_size=128,
  validation_data=(X_test, y_test),
  # callbacks=[
  #   EarlyStopping(verbose=True, patience=5, monitor='val_loss'),
  #   ModelCheckpoint(
  #     filename, monitor='val_loss',
  #     verbose=True, save_best_only=True)]
  )

p = model.predict(X_test)
print p
# pdb.set_trace()
print ('p>0.5==y_test: {:.2f}'.format(sum((p[:,0]>0.5)==y_test)/float(len(p))))

model.save('keras_simple_model_'+tag+'.h5')

# model_s2 = Sequential()
# model_s2.add(Convolution2D(
#     64, 6, 6, border_mode='same',
#     input_shape=X[0].shape))
# model_s2.add(Activation('relu'))
# model_s2.add(MaxPooling2D((2, 2), dim_ordering='th'))
# model_s2.add(Dropout(0.2))
# model_s2.add(Flatten())
# model_s2.add(Dense(128))
# model_s2.add(Activation('relu'))
# model_s2.add(Dropout(0.2))
# model_s2.add(Dense(16))
# model_s2.add(Activation('relu'))
# model_s2.add(Dense(1))
# model_s2.add(Activation('sigmoid'))
#
# model_s2.compile(
#   optimizer='rmsprop',
#   loss='binary_crossentropy',
#   metrics=['accuracy'])
#
# model_s2.fit(
#   X_train,
#   y_train,
#   nb_epoch=50,
#   batch_size=128,
#   validation_data=(X_test, y_test),
#   # callbacks=[
#   #   EarlyStopping(verbose=True, patience=5, monitor='val_loss'),
#   #   ModelCheckpoint(
#   #     filename, monitor='val_loss',
#   #     verbose=True, save_best_only=True)]
#   )
