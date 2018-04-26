#include "booksim.hpp"
#include <vector>
#include <sstream>
#include <cmath>

#include "dragontree.hpp"
#include "fattree.hpp"
#include "flatfly.hpp"
#include "misc_utils.hpp"

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
  gK = _k;
  _nodes = _k * _k;
  // _size = _k + _k / 2 + _k * _k;
  // _channels = _k * _k;
}

void DragonTree::RegisterRoutingFunctions() {}

void DragonTree::_BuildNet(const Configuration& config) {
  cout << "Dragon Tree" << endl;
  _fattree = new FatTree(config, "dragontree-fattree");
  _flatfly = new FlatFlyOnChip(config, "dragontree-flatfly");
}
