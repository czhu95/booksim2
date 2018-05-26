import sys
import re
import pickle

def extract_routing(lines):
    routing_info = []
    for line in lines:
        if line.startswith('r '):
            # clock, router, in_port, in_vc, out_port, out_vc_start, out_vc_end, out_priority
            routing_info.append([int(s) for s in line.lstrip('r ').split()])

    # print(routing_info)

    return routing_info

def extract_param(lines):
    num_vcs = 0
    vc_re = re.compile('\s*num_vcs\s*=\s*(\d+).*\n');
    for line in lines:
        m = vc_re.match(line)
        if m: num_vcs = int(m[1]); break;
    assert(num_vcs)
    print(num_vcs)

    num_switches = 0
    sw_re = re.compile('\s*#\sof\sswitches\s*=\s*(\d+).*\n');
    for line in lines:
        m = sw_re.match(line)
        if m: num_switches = int(m[1]); break;
    assert(num_switches)
    print(num_switches)

    num_ports = 0
    port_re = re.compile('\s*each\sswitch\s-\stotal\sradix\s=\s*(\d+).*\n');
    for line in lines:
        m = port_re.match(line)
        if m: num_ports = int(m[1]); break;
    assert(num_ports)
    print(num_ports)

    return num_vcs, num_switches, num_ports

def generate_training_data(info, num_vcs, num_switches, num_ports):
    cycle_info = [r[0] for r in routing_info]
    min_cycle = min(cycle_info)
    max_cycle = max(cycle_info)
    data = [[[[] for _ in range(min_cycle, max_cycle + 1)]
         for _ in range(num_ports)] for _ in range(num_switches)]
    for r in routing_info:
        # each vc in each port in each router: out_port, out_vc_start, out_vc_end, priority
        assert(data[r[1]][r[2]][r[0]-min_cycle] == [])
        data[r[1]][r[2]][r[0]-min_cycle] = [r[4], r[5], r[6], r[7]]

    return data


if __name__ == "__main__":
    with open(sys.argv[1], "r") as fid:
        lines = fid.readlines()

    routing_info = extract_routing(lines)
    num_vcs, num_switches, num_ports = extract_param(lines)

    data = generate_training_data(routing_info, num_vcs, num_switches, num_ports)

    with open('routing.pkl', 'wb') as fid:
        pickle.dump(data, fid)

    # print(data)
