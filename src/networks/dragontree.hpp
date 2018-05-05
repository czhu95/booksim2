#ifndef _DRAGONTREE_HPP_
#define _DRAGONTREE_HPP_

#include "network.hpp"

class DragonTree : public Network {

  int _k;
  Network *_fattree, *_flatfly;
  string _policy;

  void _ComputeSize(const Configuration& config);
  void _BuildNet(const Configuration& config);

public:
  DragonTree(const Configuration& config, const string& name);
  ~DragonTree();
  static void RegisterRoutingFunctions();

  virtual Credit* ReadCredit( int source );
  virtual Flit* ReadFlit( int dest );
  virtual void WriteFlit(Flit *f, int source);
  virtual void WriteCredit(Credit *c, int dest);
};

#endif


