BookSim Interconnection Network Simulator
=========================================

### Dumping routing data

Routing data are recorded in text file in log directory after running booksim for any configurations through

`./utils/run.sh ./runfiles/RUN_FILE`

log file can be processed by running python3

`python utils/routing.py log/LOG_FILE`

which will generate a python pickle file "routing.pkl" under root directory.

The pickle file contains a python nested list for all routing information:

data[router_id][in_port][cycle] = [out_port, out_vc_start, out_vc_end, priority]

### Original README

BookSim is a cycle-accurate interconnection network simulator.
Originally developed for and introduced with the [Principles and Practices of Interconnection Networks](http://cva.stanford.edu/books/ppin/) book, its functionality has since been continuously extended.
The current major release, BookSim 2.0, supports a wide range of topologies such as mesh, torus and flattened butterfly networks, provides diverse routing algorithms and includes numerous options for customizing the network's router microarchitecture.

---

If you use BookSim in your research, we would appreciate the following citation in any publications to which it has contributed:

Nan Jiang, Daniel U. Becker, George Michelogiannakis, James Balfour, Brian Towles, John Kim and William J. Dally. A Detailed and Flexible Cycle-Accurate Network-on-Chip Simulator. In *Proceedings of the 2013 IEEE International Symposium on Performance Analysis of Systems and Software*, 2013.
