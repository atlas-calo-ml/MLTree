#! /usr/bin/env python

import sys
import re

import numpy as np
from ROOT import TChain
from root_numpy import tree2array

import matplotlib.pyplot as plt
import matplotlib.pylab as pylab
import seaborn as sns
from matplotlib.colors import LogNorm

sns.set_style("whitegrid")
params = {'legend.fontsize': 'large',
      'figure.figsize': (8, 8),
      'axes.labelsize': 'x-large',
      'axes.titlesize':'large',
      'xtick.labelsize':'large',
      'ytick.labelsize':'large'}
pylab.rcParams.update(params)

def main(argv):

  # ------------------------------------------------------------------
  # Setup
  nclusters = 30
  treename = 'ClusterTree'
  input_path = '/Users/joakim/GoogleDrive/CERN/Projects/ML/MLDerivation/datasets/'
  # sample = 'pi0'
  # file_names = ['user.jolsson.mc15_13TeV.428000.ParticleGun_single_pi0_logE0p2to2000.e3496_s2139_s2132_r6569_images.20161216_4_OutputStream/user.jolsson.10234643.OutputStream._000110.root']
  sample = 'piplus'
  file_names = ['user.jolsson.mc15_13TeV.428001.ParticleGun_single_piplus_logE0p2to2000.e3501_s2141_s2132_r6569_images.20161216_4_OutputStream/user.jolsson.10234644.OutputStream._000110.root']
  # ------------------------------------------------------------------

  sample_label = '$\pi^+$'
  if sample == 'pi0':
    sample_label = '$\pi^0$'

  print '----> Converting TTree to numpy array'
  chain = TChain(treename)
  for file_name in file_names:
    chain.Add(input_path+'/'+file_name)
  layers = ['EMB1', 'EMB2', 'EMB3', 'TileBar0', 'TileBar1', 'TileBar2']
  cell_size_phi = [0.098, 0.0245, 0.0245, 0.1, 0.1, 0.1]
  cell_size_eta = [0.0031, 0.025, 0.05, 0.1, 0.1, 0.2]
  scale_factor_phi = [1, 4, 4, 1, 1, 1]
  scale_factor_eta = [8, 4, 2, 1, 1, 1]
  clusters = tree2array(chain, branches=['EMB1', 'EMB2', 'EMB3', 'TileBar0', 'TileBar1', 'TileBar2', 'clusterE'], start=0, stop=nclusters)

  print '----> Plotting images'
  for i, cluster in enumerate(clusters):
    print 'Plotting images for cluster #{}'.format(i)
    for j, layer in enumerate(layers):
      cluster_energy = '{:.2f}'.format(cluster[6])
      title = sample_label+', '+layer+', $E_{\\rm clus} = '+cluster_energy\
        +'\,{\\rm GeV}$, $\\Delta\\eta_{\\rm cell}\\times\\Delta\\phi_{\\rm cell} = '+str(cell_size_eta[j])+'\\times'+str(cell_size_phi[j])+'$'
      fig = plt.figure(figsize=(8,8))
      ax = fig.add_subplot(111)
      ax.set_title(title)
      len_phi = int(re.findall(r'\d+', str((cluster[j]).shape))[1])
      len_eta = int(re.findall(r'\d+', str((cluster[j]).shape))[0])
      phi_range = len_phi*cell_size_phi[j]/2.+1e-5
      eta_range = len_eta*cell_size_eta[j]/2.+1e-5
      plt.imshow(cluster[j], extent=[-phi_range, phi_range, -eta_range, eta_range], cmap=plt.get_cmap('Blues'), origin='lower', interpolation='nearest', norm=LogNorm(vmin=0.0001, vmax=1))
      plt.colorbar()
      plt.xlabel('$\Delta\phi$')
      plt.ylabel('$\Delta\eta$')
      plt.xlim(-phi_range, phi_range)
      plt.ylim(-eta_range, eta_range)
      major_phiticks = np.arange(-phi_range, phi_range, cell_size_phi[j]*scale_factor_phi[j])
      major_etaticks = np.arange(-eta_range, eta_range, cell_size_eta[j]*scale_factor_eta[j])
      # if j==5: #TileBar2
      #   major_etaticks = np.arange(-eta_range, eta_range, 0.2)
      minor_phiticks = np.arange(-phi_range, phi_range, cell_size_phi[j])
      minor_etaticks = np.arange(-eta_range, eta_range, cell_size_eta[j])
      ax.set_xticks(major_phiticks)
      ax.set_xticks(minor_phiticks, minor=True)
      ax.set_yticks(major_etaticks)
      ax.set_yticks(minor_etaticks, minor=True)
      # ax.grid(which='both')
      ax.grid(which='minor', alpha=0.2)
      ax.grid(which='major', alpha=0.5)
      fig.savefig('image_'+sample+'_cluster_{}_'.format(i)+layer+'.pdf', bbox_inches='tight')

if __name__ == "__main__":
  main(sys.argv)
