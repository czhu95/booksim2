
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


available_choices = list(np.arange(0.01, 0.39, 0.01))
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

print find_sat(sys.argv[1], sys.argv[2], sys.argv[3])
# print analyze_m(sys.argv[1])
