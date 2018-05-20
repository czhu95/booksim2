
import os
import numpy as np
import sys
import pickle

def test(config_file, inj_rate, traffic, policy, ps):
    os.system('cp %s %s.tmp'%(config_file, config_file))
    os.system('echo "injection_rate = %.2f;" >> %s.tmp'%(inj_rate, config_file))
    os.system('echo "traffic = %s;" >> %s.tmp'%(traffic, config_file))
    os.system('echo "injection_policy= %s;" >> %s.tmp'%(policy, config_file))
    os.system('echo "packet_size = %d;" >> %s.tmp'%(ps, config_file))
    a = os.system('./booksim %s.tmp > output.tmp'%config_file)
    res = open('output.tmp').read()
    if 'Aborting' in res:
        return 1
    else:
        return 0

def analyze_m(mfile):
    lines = open(mfile).readlines()
    for line in lines:
        if line.startswith('plat_hist'):
            plat_hist_line = line
            break
    plat_hist = map(float, plat_hist_line[plat_hist_line.index('[')+2:plat_hist_line.index(']')-1].split())
    plat_hist = np.array(plat_hist)
    len_p = len(plat_hist)
    plat_csum = np.cumsum(plat_hist)
    plat_sum = np.sum(plat_hist)
    vals = np.arange(len_p) + 1
    avg = np.dot(vals, plat_hist) / plat_sum
    med = np.where(plat_csum >= plat_sum / 2)[0][0] + 1
    p99 = np.where(plat_csum >= plat_sum * 0.99)[0][0] + 1
    std = np.sqrt(np.dot(plat_hist, vals * vals) / plat_sum - avg * avg)
    max_ = np.where(plat_hist > 0)[0][-1]
    return avg, med, p99, std, max_


traffics = ['uniform', 'bitcomp', 'tornado', 'neighbor']
policys = ['fattree', 'flatfly','oblivious']
packets = [2, 2, 4]
inj_rates = np.arange(0.1, 1.1, 0.1)
def test_stats(config_file):
    results_avg = {}
    results_max = {}
    for traffic in traffics:
        results_avg[traffic] = {}
        results_max[traffic] = {}
        for ps, policy in zip(packets, policys):
            max_lat = []
            avg_lat = []
            for inj_rate in inj_rates / ps:
                flag = test(config_file, inj_rate, traffic, policy, ps)
                if flag != 0:
                    print traffic, policy, inj_rate, ps
                    break
                results = analyze_m('dragontree.m')
                max_lat.append(results[4])
                avg_lat.append(results[0])
            results_avg[traffic][policy] = avg_lat
            results_max[traffic][policy] = max_lat
    pickle.dump((results_avg, results_max), open('output7.pkl','w'))

test_stats(sys.argv[1])
