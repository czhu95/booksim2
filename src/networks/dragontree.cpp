#include "booksim.hpp"
#include <vector>
#include <sstream>
#include <cmath>

#include "dragontree.hpp"
#include "fattree.hpp"
#include "flatfly_onchip.hpp"
#include "misc_utils.hpp"
#include "../booksim_config.hpp"
#include "../random_utils.hpp"

DragonTree::DragonTree(const Configuration& config, const string& name)
    : Network(config, name) {
  _ComputeSize(config);
  _Alloc();
  _BuildNet(config);
  nn = 2;
}

DragonTree::~DragonTree() {
  for (int i = 0; i < nn; i++) {
    delete _nets[i];
  }
}

void DragonTree::_ComputeSize(const Configuration& config) {
  _k = config.GetInt("k");
  int _n = config.GetInt("n");
  gK = 0;
  _nodes = powi( _k, _n );
  _size = 0;
  _channels = 0;
}

void DragonTree::RegisterRoutingFunctions() {
  gRoutingFunctionMap["ran_min_dragontree"] = &min_flatfly;
}

void DragonTree::_BuildNet(const Configuration& config) {
  Network* _fattree, *_flatfly;
  Configuration config_ = Configuration(config);
  cout << "Build ftree" << endl;
  config_.AddStrField("topology", "fattree");
  config_.AddStrField("routing_function", "dnca");
  _fattree = Network::New(config_, "dragontree-fattree");
  _nets.push_back(_fattree);
  
  cout << "Build ffly" << endl;
  config_.AddStrField("topology", "flatfly");
  config_.AddStrField("routing_function", "xyyx");
  _flatfly = Network::New(config_, "dragontree-flatfly");
  _nets.push_back(_flatfly);

  config_.AddStrField("topology", "dragontree");
  _timed_modules.push_back(_fattree);
  _timed_modules.push_back(_flatfly);

  _policy = config.GetStr("injection_policy");

}

void DragonTree::WriteFlit(Flit *f, int source) {
  int real_source, net_id;
  real_source = source % _nodes;
  net_id = source / _nodes;
  _nets[net_id]->WriteFlit(f, real_source);

}

void DragonTree::WriteCredit(Credit *c, int dest) {
  int real_dest, net_id;
  real_dest = dest % _nodes;
  net_id = dest / _nodes;
  _nets[net_id]->WriteCredit(c, real_dest);
  
}

Flit* DragonTree::ReadFlit( int dest ) {
  int real_dest, net_id;
  real_dest = dest % _nodes;
  net_id = dest / _nodes;
  return _nets[net_id]->ReadFlit(real_dest);
}

Credit* DragonTree::ReadCredit( int source ) {
  int real_source, net_id;
  real_source = source % _nodes;
  net_id = source / _nodes;
  return _nets[net_id]->ReadCredit(real_source);
}

int DragonTree::ChooseSubnet(const Flit* f, int source) const {
  assert(f->head);
  assert(f->src == source);
  int chosen;
  if (_policy == "deterministic") {
    chosen = (f->src + f->dest) % 2;
  }
  else if (_policy == "flatfly") {
    chosen = 1;
  }
  else if (_policy == "fattree") {
    chosen = 0;
  }
  else if (_policy == "oblivious") {
    chosen = RandomInt(1);
  }
  else {
    assert(false);
  }
  return chosen;
}

