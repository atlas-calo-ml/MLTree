#! /usr/bin/env python

import sys
import re

import numpy as np
from ROOT import TFile, TChain
from root_numpy import root2array, tree2array, fill_hist

import matplotlib.pyplot as plt
import matplotlib.pylab as pylab
import seaborn as sns
from matplotlib.colors import LogNorm

sns.set_style("whitegrid")
params = {'legend.fontsize': 'x-large',
          'figure.figsize': (8, 8),
         'axes.labelsize': 'x-large',
         'axes.titlesize':'x-large',
         'xtick.labelsize':'large',
         'ytick.labelsize':'large'}
pylab.rcParams.update(params)

def main(argv):

    # ------------------------------------------------------------------
    # Setup
    nevents = 20
    input_path = '/Users/joakim/GoogleDrive/CERN/Projects/ML/MLDerivation/test_datasets/'
    file_names = ['MLderivation.pool.root']
    treename = 'ImageTree'
    sample = 'piplus' # 'piplus' or 'pi0'
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
    layer_labels = [sample_label+', EMB1 (cell size: $\\Delta\\eta\\times\\Delta\\phi = 0.0031\\times0.0980$)',
                    sample_label+', EMB2 (cell size: $\\Delta\\eta\\times\\Delta\\phi = 0.0250\\times0.0245$)',
                    sample_label+', EMB3 (cell size: $\\Delta\\eta\\times\\Delta\\phi = 0.0500\\times0.0245$)',
                    sample_label+', TileBar0 (cell size: $\\Delta\\eta\\times\\Delta\\phi = 0.1\\times0.1$)',
                    sample_label+', TileBar1 (cell size: $\\Delta\\eta\\times\\Delta\\phi = 0.1\\times0.1$)',
                    sample_label+', TileBar2 (cell size: $\\Delta\\eta\\times\\Delta\\phi = 0.2\\times0.1$)'
                    ]
    clusters = tree2array(chain, branches=layers, start=0, stop=nevents)

    print '----> Plotting images'
    for i, cluster in enumerate(clusters):
        print 'Event {}'.format(i)
        for j, layer in enumerate(layers):
            fig = plt.figure(figsize=(8,8))
            ax = fig.add_subplot(111)
            ax.set_title(layer_labels[j])
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
            #     major_etaticks = np.arange(-eta_range, eta_range, 0.2)
            minor_phiticks = np.arange(-phi_range, phi_range, cell_size_phi[j])
            minor_etaticks = np.arange(-eta_range, eta_range, cell_size_eta[j])
            ax.set_xticks(major_phiticks)
            ax.set_xticks(minor_phiticks, minor=True)
            ax.set_yticks(major_etaticks)
            ax.set_yticks(minor_etaticks, minor=True)
            # ax.grid(which='both')
            ax.grid(which='minor', alpha=0.2)
            ax.grid(which='major', alpha=0.5)
            fig.savefig('image_'+sample+'_event_{}_'.format(i)+layer+'.pdf', bbox_inches='tight')

if __name__ == "__main__":
    main(sys.argv)
