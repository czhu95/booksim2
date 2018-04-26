#ifndef _DRAGONTREE_HPP_
#define _DRAGONTREE_HPP_

#include "network.hpp"

class DragonTree : public Network {

  int _k;
  Network *_fattree_net, *_flatfly_net;

  void _ComputeSize(const Configureation& config);
  void _BuildNet(const Configuration& config);

public:
  DragonTree(const Configuration& config, const string& name);
  ~DragonTree();
  static void RegisterRoutingFunctions();
};

#endif


