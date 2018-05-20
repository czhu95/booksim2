import numpy as np
import pickle
import os
import matplotlib.pyplot as plt

results_avg, results_max = pickle.load(open('output7.pkl'))
x = np.arange(0.1, 1.1, 0.1)
os.system('mkdir p7')
ps = {'dragontree':4, 'flatfly':2, 'fattree':2}
for traffic in results_avg:
    for idx, results_toshow in enumerate([results_avg, results_max]):
        type_ = 'average' if idx == 0 else 'max'
        for policy in results_toshow[traffic]:
            y = results_toshow[traffic][policy]
            tmpx = x[:len(y)]
            if policy == 'oblivious':
                policy = 'dragontree'
            plt.plot(tmpx / ps[policy], y, label=policy)
        plt.legend()
        plt.xlabel(traffic)
        plt.ylabel(type_ + ' latency')
        plt.savefig('p7/%s_%s.pdf'%(traffic, type_))
        plt.clf()
