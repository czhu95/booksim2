import sys
import pickle as pkl
import torch
import torch.nn as nn
from torch.autograd import Variable

N = 10;

# if __name__ == "__main__":
if True:
    with open(sys.argv[1], 'rb') as fid:
        routing_info = pkl.load(fid)

    all_data = [port for router in routing_info for port in router]
    input_data = []
    target_data = []
    for port_data in all_data:
        for i in range(len(port_data) - N - 1):
            input_data.append([port_data[j] for j in range(i, i + 10)])
            target_data.append(port_data[i + 10])

    input_data = [[x[0] if x else -1 for x in r] for r in input_data]
    target_data = [y[0] if y else -1 for y in target_data]
    import pdb; pdb.set_trace()
    perceptron = nn.Linear(N, 2)

