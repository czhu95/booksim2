// $Id$

/*
 Copyright (c) 2007-2015, Trustees of The Leland Stanford Junior University
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

////////////////////////////////////////////////////////////////////////
//
// FatTree
//
//       Each level of the hierarchical indirect Network has
//       k^(n-1) Routers. The Routers are organized such that
//       each node has k descendents, and each parent is
//       replicated k  times.
//      most routers has 2K ports, excep the top level has only K
////////////////////////////////////////////////////////////////////////
//
// RCS Information:
//  $Author: jbalfour $
//  $Date: 2007/06/26 22:50:48 $
//  $Id$
//
////////////////////////////////////////////////////////////////////////


#include "booksim.hpp"
#include <vector>
#include <sstream>
#include <cmath>

#include "fattree.hpp"
#include "misc_utils.hpp"


 //#define FATTREE_DEBUG

FatTree::FatTree( const Configuration& config,const string & name )
  : Network( config ,name)
{


  _ComputeSize( config );
  _Alloc( );
  _BuildNet( config );

}

void FatTree::_ComputeSize( const Configuration& config )
{

  _k = config.GetInt( "k" );
  _n = config.GetInt( "n" );
  _t = config.GetInt( "t" );

  gK = _k; gN = _n;

  _nodes = powi( _k, _n );

  if (_t == 1) {
    //levels * routers_per_level
    _size = _n * powi( _k , _n - 1 );
    //(channels per level = k*routers_per_level* up/down) * (levels-1)
    _channels = (2*_k * powi( _k , _n-1 ))*(_n-1);
  } else {
    _size = powi(_k, _n - 1) * (powi(_t, _n) - 1) / (powi(_t, _n) - powi(_t, _n - 1));
    _channels = 2 * _k * (_size - powi(_k, _n - 1));
  }
}


void FatTree::RegisterRoutingFunctions() {

}

void FatTree::_BuildNet( const Configuration& config )
{
 cout << "Fat Tree" << endl;
  cout << " k = " << _k << " levels = " << _n << endl;
  cout << " each switch - total radix =  "<< 2*_k << endl;
  cout << " # of switches = "<<  _size << endl;
  cout << " # of channels = "<<  _channels << endl;
  cout << " # of nodes ( size of network ) = " << _nodes << endl;


  // Number of router positions at each depth of the network
  const int nPos = powi( _k, _n-1);
  assert(_n == 2);
  assert(_t == 2);

  //
  // Allocate Routers
  //
  ostringstream name;
  int level, pos, id, degree, port;
  for ( level = 0 ; level < _n ; ++level ) {
    int level_num;
    if (level == 0) 
      level_num = nPos / 2;
    else
      level_num = nPos;
    for ( pos = 0 ; pos < level_num; ++pos ) {

      if ( level == 0 ) { //top routers is zero
        degree = _k;
        id = pos;
      }
      else {
        degree = 3 * _k / 2;
        id = nPos / 2 + pos;
      }

      name.str("");
      name << "router_level" << level << "_" << pos;
      Router * r = Router::NewRouter( config, this, name.str( ), id,
				      degree, degree );
      _Router( level, pos ) = r;
      _timed_modules.push_back(r);
    }
  }

  //
  // Connect Channels to Routers
  //

  //
  // Router Connection Rule: Output Ports <gK Move DOWN Network
  //                         Output Ports >=gK Move UP Network
  //                         Input Ports <gK from DOWN Network
  //                         Input Ports >=gK  from up Network

  // Connecting  Injection & Ejection Channels
  for ( pos = 0 ; pos < nPos ; ++pos ) {
    for(int index = 0; index<_k; index++){
      int link = pos*_k + index;
      _Router( _n-1, pos)->AddInputChannel( _inject[link],
					    _inject_cred[link]);
      _Router( _n-1, pos)->AddOutputChannel( _eject[link],
					     _eject_cred[link]);
      _inject[link]->SetLatency( 1 );
      _inject_cred[link]->SetLatency( 1 );
      _eject[link]->SetLatency( 1 );
      _eject_cred[link]->SetLatency( 1 );
    }
  }

#ifdef FATTREE_DEBUG
  cout<<"\nAssigning output\n";
#endif

  //connect all down output channels
  //level n-1's down channel are injection channels
  int link_count = 0, last_link_count = 0;
  { level = 0;
    for ( pos = 0; pos < nPos / 2; ++pos ) {
      for ( port = 0; port < _k; ++port ) {
	int link = pos*_k + port;
  link_count++;
	_Router(level, pos)->AddOutputChannel( _chan[link],
						_chan_cred[link] );
  _Router(level+1, port)->AddInputChannel( _chan[link],
            _chan_cred[link] );
	_chan[link]->SetLatency( 1 );
	_chan_cred[link]->SetLatency( 1 );
#ifdef FATTREE_DEBUG
	cout<<_Router(level, pos)->Name()<<" "
	    <<"down output "<<port<<" "
	    <<"channel_id "<<link<<endl;
#endif

      }
    }
  }
  last_link_count = link_count;
  assert(last_link_count == _k * _k / 2);
  //connect all up output channels
  //level 0 has no up chnanels
  { level = 1;
    for ( pos = 0; pos < nPos; ++pos ) {
      for ( port = 0; port < _k/2; ++port ) {
	int link = last_link_count + pos*(_k/2) + port ;
  link_count++;
	_Router(level, pos)->AddOutputChannel( _chan[link],
						_chan_cred[link] );
	_Router(level-1, port)->AddInputChannel( _chan[link],
						_chan_cred[link] );
	_chan[link]->SetLatency( 1 );
	_chan_cred[link]->SetLatency( 1 );
#ifdef FATTREE_DEBUG
	cout<<_Router(level, pos)->Name()<<" "
	    <<"up output "<<port<<" "
	    <<"channel_id "<<link<<endl;
#endif
      }
    }
  }
  last_link_count = link_count;
  assert(last_link_count == _k * _k);

#ifdef FATTREE_DEBUG
  cout<<"\nChannel assigned\n";
#endif
}

Router*& FatTree::_Router( int depth, int pos )
{
  return _routers[depth * _k / 2 + pos];
}
