import numpy as np
import os
import matplotlib.pyplot as plt

os.system('mkdir p6')
for policy in ['deterministic', 'oblivious', 'adaptive']:
    lines = open('flit_cycle.info.'+policy).readlines()
    infos = np.array(map(lambda x:map(int, x.split()), lines))
    cycles = np.arange(np.max(infos[:,0])+1)
    flits = np.zeros(len(cycles))
    for info in infos:
        flits[info[0]] += 1

    plt.plot(flits)
    plt.xlabel('%s-whole'%policy)
    plt.savefig('p6/%s_whole.png'%policy)
    plt.clf()

    plt.plot(np.arange(600) + 9700, flits[9700:10300])
    plt.xlabel('%s-near-10000'%policy)
    plt.savefig('p6/%s_near.png'%policy)
    plt.clf()
