
import os
import numpy as np
import sys

def test(config_file, inj_rate, traffic, policy):
    os.system('cp %s %s.tmp'%(config_file, config_file))
    os.system('echo "injection_rate = %.2f;" >> %s.tmp'%(inj_rate, config_file))
    os.system('echo "traffic = %s;" >> %s.tmp'%(traffic, config_file))
    os.system('echo "injection_policy= %s;" >> %s.tmp'%(policy, config_file))
    a = os.system('./booksim %s.tmp > output.tmp'%config_file)
    res = open('output.tmp').read()
    if 'Aborting' in res:
        return 1
    else:
        return 0

available_choices = list(np.arange(0.01, 0.26, 0.01))
def find_sat(config_file, traffic, policy):
    start_point = 0
    end_point = len(available_choices)
    while start_point < end_point - 1:
        mid_point = (start_point + end_point) // 2
        inj_rate = available_choices[mid_point]
        flag = test(config_file, inj_rate, traffic, policy)
        if flag == 0:
            start_point = mid_point
        else:
            end_point = mid_point
    if flag != 0:
        flag = test(config_file, available_choices[start_point], traffic, policy)
    if flag != 0:
        print "1% inj rate does not satisfy"
        print traffic, policy
    else:
        return available_choices[start_point]

def analyze_m(mfile):
    lines = open(mfile).readlines()
    plat_hist_line = lines[116]
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
    return avg, med, p99, std


traffics = ['uniform', 'bitcomp', 'transpose', 'neighbor']
policys = ['deterministic', 'oblivious', 'adaptive']
def test_stats(config_file):
    f = open('output.csv','w')
    f.write('saturate inj rate, traffic pattern, policy, average, median, 99% point, std\n')
    for traffic in traffics:
        deterministic_inj_rate = 0.0
        for policy in policys:
            inj_rate = find_sat(config_file, traffic, policy)
            if policy == 'deterministic':
                deterministic_inj_rate = inj_rate
            else:
                assert inj_rate >= deterministic_inj_rate
            flag = test(config_file, deterministic_inj_rate, traffic, policy)
            assert flag == 0, '%.2f, %s, %s'%(inj_rate, traffic, policy)
            results = analyze_m('dragontree.m')
            f.write('%.2f, %s, %s, %f, %f, %f, %f\n'%((inj_rate, traffic, policy) + results))
            print '%.2f, %s, %s, %f, %f, %f, %f\n'%((inj_rate, traffic, policy) + results)

test_stats(sys.argv[1])
