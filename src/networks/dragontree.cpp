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
  delete _fattree, _flatfly;
  _fattree = NULL;
  _flatfly = NULL;
}

void DragonTree::_ComputeSize(const Configuration& config) {
  _k = config.GetInt("k");
  gK = 0;
  _nodes = 0;
  _size = 0;
  _channels = 0;
}

void DragonTree::RegisterRoutingFunctions() {}

void DragonTree::_BuildNet(const Configuration& config) {
  BookSimConfig config_ftree, config_ffly;
  cout << "Parse ftree" << endl;
  config_ftree.ParseFile("config_ftree.config");
  _fattree = new FatTree(config_ftree, "dragontree-fattree");
  
  cout << "Parse ffly" << endl;
  config_ffly.ParseFile("config_ffly.config");
  _flatfly = new FlatFlyOnChip(config_ffly, "dragontree-flatfly");

}

void DragonTree::WriteFlit(Flit *f, int source) {
  _fattree->WriteFlit(f, source);
}

void DragonTree::WriteCredit(Credit *c, int dest) {
  _fattree->WriteCredit(c, dest);
}

Flit* DragonTree::ReadFlit( int dest ) {
  return _fattree->ReadFlit(dest);
}

Credit* DragonTree::ReadCredit( int source ) {
  return _fattree->ReadCredit(source);
}
