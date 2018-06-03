import sys
import pickle as pkl
import numpy as np
import torch
import torch.nn as nn
from torch.autograd import Variable
import argparse

N = 10
bs = 64
epochs = 1

# if __name__ == "__main__":
def get_data(data_file):
    with open(data_file, 'rb') as fid:
        routing_info = pkl.load(fid)

    all_data = [port for router in routing_info for port in router]
    input_data = []
    target_data = []
    for port_data in all_data:
        for i in range(len(port_data) - N - 1):
            input_data.append([port_data[j] for j in range(i, i + N)])
            target_data.append(port_data[i + 10])

    input_data = [[x[0] if x else -1 for x in r] for r in input_data]
    target_data = [y[0] if y else -1 for y in target_data]
    return np.array(input_data), np.array(target_data)

def represent_data(data, label):
    num_ports = np.max(label) + 1
    num_points = data.shape[0]
    window_len = data.shape[1]
    onehot_data = np.zeros((num_points * window_len, num_ports + 1), 'f')
    onehot_label = np.zeros((num_points, num_ports + 1), 'f')
    onehot_data[np.arange(num_points * window_len), 1 + data.flatten()] = 1
    onehot_data = onehot_data.reshape((num_points, window_len * (num_ports + 1)))
    
    onehot_label[np.arange(num_points), 1 + label] = 1
    return onehot_data, onehot_label

def partition_data(data, label):
    num_points = data.shape[0]
    idxs = np.arange(num_points)
    np.random.shuffle(idxs)
    train_idxs = idxs[:int(num_points * 0.7)]
    val_idxs = idxs[int(num_points * 0.7):]
    return (data[train_idxs], label[train_idxs]), (data[val_idxs], label[val_idxs])

class Perceptron(nn.Module):
    def __init__(self, input_dim, output_dim, layers = 1):
        super(Perceptron, self).__init__()
        dims = [input_dim] + [32] * (layers - 1) + [output_dim]
        modules = [nn.Linear(x,y) for x,y in zip(dims, dims[1:])]
        self.output = nn.Sequential(*modules)

    def forward(self, inputs):
        return self.output(inputs)

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('data')
    parser.add_argument('--layers', type=int, default=1)
    args = parser.parse_args()

    data, label = get_data(args.data)
    num_ports = int(np.max(label) + 1)
    data, _ = represent_data(data, label)
    label = label + 1
    model = Perceptron(N * (num_ports + 1), num_ports + 1, args.layers).cuda()
    print("model spec")
    print("Input:", N * (num_ports + 1))
    print("Output:", num_ports + 1)
    print("Layers:", args.layers)
    criterion = nn.CrossEntropyLoss().cuda()
    optimizer = torch.optim.SGD(model.parameters(), 1e-2,
            weight_decay=0.005)

    train_split, val_split = partition_data(data, label)
    data_train, label_train = train_split
    data_val, label_val = val_split

    for i in range(epochs):
        print("epoch ", i)
        train(data_train, label_train, model, criterion, optimizer)
        validate(data_val, label_val, model, criterion)
    
def validate(data, label, model, criterion):
    num_points = len(data)
    losses = AverageMeter()
    top1 = AverageMeter()
    for i in range(num_points // bs):
        inputs = torch.from_numpy(data[i * bs:i * bs + bs]).cuda()
        target = torch.from_numpy(label[i * bs:i * bs + bs]).cuda()
        output = model(inputs)
        loss = criterion(output, target)
    
        prec1 = accuracy(output.data, target)
        losses.update(loss.data.item(), inputs.size(0))
        top1.update(prec1[0].item(), inputs.size(0))
    print('val')
    print('top1:', top1.avg)
    print('loss:', losses.avg)

def train(data, label, model, criterion, optimizer):

    num_points = len(data)
    idxs = np.arange(num_points)
    np.random.shuffle(idxs)
    data = data[idxs]
    label = label[idxs]
    
    top1 = AverageMeter()
    losses = AverageMeter()
    for i in range(num_points // bs):
        inputs = torch.from_numpy(data[i * bs:i * bs + bs]).cuda()
        target = torch.from_numpy(label[i * bs:i * bs + bs]).cuda()
        output = model(inputs)
        loss = criterion(output, target)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        prec1 = accuracy(output.data, target)
        losses.update(loss.data.item(), inputs.size(0))
        top1.update(prec1[0].item(), inputs.size(0))
    print('train')
    print('top1:', top1.avg)
    print('loss:', losses.avg)

def accuracy(output, target, topk=(1,)):
    """Computes the precision@k for the specified values of k"""
    batch_size = target.size(0)
    num = output.size(1)
    target_topk = []
    appendices = []
    for k in topk:
        if k <= num:
            target_topk.append(k)
        else:
            appendices.append([0.0])
    topk = target_topk
    maxk = max(topk)
    _, pred = output.topk(maxk, 1, True, True)
    pred = pred.t()
    correct = pred.eq(target.view(1, -1).expand_as(pred))

    res = []
    for k in topk:
        correct_k = correct[:k].view(-1).float().sum(0)
        res.append(correct_k.mul_(100.0 / batch_size))
    return res + appendices

class AverageMeter(object):
    """Computes and stores the average and current value"""
    def __init__(self):
        self.reset()

    def reset(self):
        self.val = 0
        self.avg = 0
        self.sum = 0
        self.count = 0

    def update(self, val, n=1):
        self.val = val
        self.sum += val * n
        self.count += n
        self.avg = self.sum / self.count


'''
def adjust_learning_rate(optimizer, epoch, step_size):
    """Sets the learning rate to the initial LR decayed by 10 every 30 epochs"""
    lr = args.lr * (0.1 ** (epoch // step_size))
    for param_group in optimizer.param_groups:
        param_group['lr'] = lr

'''
main()
