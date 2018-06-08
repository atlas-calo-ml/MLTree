#! /usr/bin/env python
import sys
import numpy as np

def main(argv):

  # ------------------------------------------------------------------
  # Setup
  nclusters_max = 333000 # from each of pi0 and piplus
  # nclusters_max = 1 # from each of pi0 and piplus
  input_path = '/Users/joakim/GoogleDrive/CERN/Projects/ML/MLDerivation/datasets/'
  pi0_folder = 'user.jolsson.mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.e3496_s2139_s2132_r6569_images.20161216_4_OutputStream/'
  piplus_folder = 'user.jolsson.mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.e3501_s2141_s2132_r6569_images.20161216_4_OutputStream/'
  # ------------------------------------------------------------------

  from os import listdir
  from os.path import isfile, join
  pi0_files = [join(input_path+pi0_folder,f) for f in listdir(input_path+pi0_folder)]
  piplus_files = [join(input_path+piplus_folder,f) for f in listdir(input_path+piplus_folder)]

  print ('----> Loading input trees')
  clusters_pi0 = load_tree(pi0_files, nclusters_max)
  clusters_piplus = load_tree(piplus_files, nclusters_max)

  print ('----> Preparing arrays for Keras')
  layers_array_pi0 = prepare_keras_input(clusters_pi0, 0)
  layers_array_piplus = prepare_keras_input(clusters_piplus, 1)

  EMB2_pi0 = layers_array_pi0[1]
  EMB2_piplus = layers_array_piplus[1]
  EMB2 = np.append(EMB2_pi0, EMB2_piplus, axis=0)

  EMB2_flat_pi0 = layers_array_pi0[6]
  EMB2_flat_piplus = layers_array_piplus[6]
  EMB2_flat = np.append(EMB2_flat_pi0, EMB2_flat_piplus, axis=0)

  all_pixels_pi0 = layers_array_pi0[7]
  all_pixels_piplus = layers_array_piplus[7]
  all_pixels = np.append(all_pixels_pi0, all_pixels_piplus, axis=0)

  y_pi0 = layers_array_pi0[8]
  y_piplus = layers_array_piplus[8]
  y = np.append(y_pi0, y_piplus, axis=0)

  # print ('Before shuffle:')
  # print ('EMB2_flat', EMB2_flat)
  # print ('EMB2_flat.shape', EMB2_flat.shape)
  # print ('all_pixels', all_pixels)
  # print ('all_pixels.shape', all_pixels.shape)
  # print ('y',y)

  from sklearn.utils import shuffle
  all_pixels, EMB2, EMB2_flat, y = shuffle(all_pixels, EMB2, EMB2_flat, y, random_state=0)
  y = y[:,0,0]

  # print ('After shuffle:')
  # print ('EMB2_flat', EMB2_flat)
  # print ('EMB2_flat.shape', EMB2_flat.shape)
  # print ('all_pixels', all_pixels)
  # print ('all_pixels.shape', all_pixels.shape)
  # print ('y',y)

  print ('----> Array sizes')
  print ('all_pixels.shape', all_pixels.shape)
  print ('EMB2.shape', EMB2.shape)
  print ('EMB2_flat.shape', EMB2_flat.shape)
  print ('y.shape', y.shape)

  print ('----> Saving arrays to file')
  np.save('array_all_pixels.npy', all_pixels)
  np.save('array_EMB2.npy', EMB2)
  np.save('array_EMB2_flat.npy', EMB2_flat)
  np.save('array_y.npy', y)

  ## for sanity check, random labels should yield 50-50 result
  # import random
  # random.shuffle(y)
  # np.save('array_random_y.npy', y)

  print ('----> All done!')

def prepare_keras_input(clusters, sample_class):
  nclusters = len(clusters)
  # Arrays for each layer
  EMB1 = np.zeros((nclusters, 4, 128, 1))
  EMB2 = np.zeros((nclusters, 16, 16, 1))
  EMB3 = np.zeros((nclusters, 16, 8, 1))
  TileBar0 = np.zeros((nclusters, 4, 4, 1))
  TileBar1 = np.zeros((nclusters, 4, 4, 1))
  TileBar2 = np.zeros((nclusters, 4, 2, 1))

  # A vector with all pixels (cells) in a cluster and total cluster energy
  all_pixels = np.zeros((nclusters, 1 + 4*128 + 16*16 + 16*8 + 4*4 + 4*4 + 4*2, 1))
  EMB2_flat = np.zeros((nclusters, 1 + 16*16, 1))
  pixel_i = [1, 513, 769, 897, 913, 929, 937]

  layers_labels = ['EMB1', 'EMB2', 'EMB3', 'TileBar0', 'TileBar1', 'TileBar2', 'EMB2_flat', 'all_pixels', 'truth_labels']
  layers_array = [EMB1, EMB2, EMB3, TileBar0, TileBar1, TileBar2]

  for i, cluster in enumerate(clusters):
    if i%1000 == 0 and i > 0:
      print ('Cluster #{}'.format(i))
    all_pixels[i][0][0] = cluster[6] # fill total cluster energy
    EMB2_flat[i][0][0] = cluster[6] # fill total cluster energy
    for j in xrange(len(layers_array)):
      cluster_transpose = np.transpose(np.array([cluster[j]]))
      layers_array[j][i] = cluster_transpose
      all_pixels[i][pixel_i[j]:pixel_i[j+1]] = np.array([cluster[j].flatten()]).transpose()
      if j == 1:
        EMB2_flat[i][1:257] = np.array([cluster[j].flatten()]).transpose()

  layers_array.append(EMB2_flat)
  layers_array.append(all_pixels)
  layers_array.append(np.full((nclusters, 1, 1), sample_class))
  return layers_array

def load_tree(file_paths, nclusters_max = -1, treename = 'ClusterTree'):
  from ROOT import TChain
  chain = TChain(treename)
  for file_path in file_paths:
    chain.Add(file_path)
  from root_numpy import tree2array
  clusters = tree2array(chain, branches=['EMB1', 'EMB2', 'EMB3', 'TileBar0', 'TileBar1', 'TileBar2', 'clusterE'], start=0, stop=nclusters_max)
  return clusters

if __name__ == "__main__":
  main(sys.argv)
