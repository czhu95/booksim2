#include "booksim.hpp"
#include <vector>
#include <sstream>
#include <cmath>

#include "dragontree.hpp"
#include "fattree.hpp"
#include "flatfly_onchip.hpp"
#include "misc_utils.hpp"
#include "../booksim_config.hpp"

DragonTree::DragonTree(const Configuration& config, const string& name)
    : Network(config, name) {
  _ComputeSize(config);
  _Alloc();
  _BuildNet(config);
}

DragonTree::~DragonTree() {
  delete _fattree;
  delete _flatfly;
  _fattree = NULL;
  _flatfly = NULL;
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
  Configuration config_ = Configuration(config);
  cout << "Build ftree" << endl;
  config_.AddStrField("topology", "fattree");
  config_.AddStrField("routing_function", "nca");
  _fattree = Network::New(config_, "dragontree-fattree");
  
  cout << "Build ffly" << endl;
  config_.AddStrField("topology", "flatfly");
  config_.AddStrField("routing_function", "xyyx");
  _flatfly = Network::New(config_, "dragontree-flatfly");
  config_.AddStrField("topology", "dragontree");

  _timed_modules.push_back(_fattree);
  _timed_modules.push_back(_flatfly);

  _policy = config.GetStr("injection_policy");

}

void DragonTree::WriteFlit(Flit *f, int source) {
  if (_policy == "flatfly") {
    _flatfly->WriteFlit(f, source);
  }
  else if (_policy == "fattree") {
    _fattree->WriteFlit(f, source);
  }
  else {
    cerr << "Unknown injection policy" << endl;
    exit(-1);
  }

}

void DragonTree::WriteCredit(Credit *c, int dest) {
  if (_policy == "flatfly") {
    _flatfly->WriteCredit(c, dest);
  }
  else if (_policy == "fattree") {
    _fattree->WriteCredit(c, dest);
  }
  else {
    cerr << "Unknown injection policy" << endl;
    exit(-1);
  }
}

Flit* DragonTree::ReadFlit( int dest ) {
  Flit* flit_;
  if (_policy == "flatfly") {
    flit_ = _flatfly->ReadFlit(dest);
  }
  else if (_policy == "fattree") {
    flit_ = _fattree->ReadFlit(dest);
  }
  else {
    cerr << "Unknown injection policy" << endl;
    exit(-1);
  }
  return flit_;
}

Credit* DragonTree::ReadCredit( int source ) {
  Credit* credit_;
  if (_policy == "flatfly") {
    credit_ = _flatfly->ReadCredit(source);
  }
  else if (_policy == "fattree") {
    credit_ = _fattree->ReadCredit(source);
  }
  else {
    cerr << "Unknown injection policy" << endl;
    exit(-1);
  }
  return credit_;
}
