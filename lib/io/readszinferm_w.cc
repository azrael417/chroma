// $Id: readszinferm_w.cc,v 1.5 2003-12-30 17:27:15 bjoo Exp $: readszinqprop_w.cc,v 1.6 2003/04/30 21:19:33 edwards Exp $
/*!
 * @file
 * @brief  Read an old SZIN-style (checkerboarded) lattice Dirac fermion
 */

#include "chromabase.h"
#include "io/readszinferm_w.h"

#include "qdp_util.h"   // from QDP++

using namespace QDP;

//! Read a SZIN fermion. This is a simple memory dump reader.
/*!
 * \ingroup io
 *
 * \param q          lattice fermion ( Modify )
 * \param file       path ( Read )
 */    

void readSzinFerm(LatticeFermion& q, const string& file)
{
  BinaryReader cfg_in(file);

  multi1d<Real64>  buffer(Ns*Nc*2);  // Buffer for a fermion

  //
  // Read propagator field
  //
  multi1d<int> lattsize_cb = Layout::lattSize();
  lattsize_cb[0] /= 2;  // checkerboard in the x-direction in szin

  // Read prop
  for(int cb=0; cb < 2; ++cb)
  {
    for(int sitecb=0; sitecb < Layout::vol()/2; ++sitecb)
    {
      multi1d<int> coord = crtesn(sitecb, lattsize_cb);

      // construct the checkerboard offset
      int sum = 0;
      for(int m=1; m<Nd; m++)
	sum += coord[m];

      // The true lattice x-coord
      coord[0] = 2*coord[0] + ((sum + cb) & 1);


      read(cfg_in, buffer, Nc*Ns*2);
      Fermion q_tmp;
      int bufindex = 0;
      

      for(int spin=0; spin < Ns; ++spin) {
	ColorVector tmp_vec;
	
	for(int col = 0; col < Nc; ++col) {
	  Complex tmp_cmpx;
	  Real64 re, im;

	  re = buffer[bufindex];
	  bufindex++;
	  im = buffer[bufindex];
	  bufindex++;
	  tmp_cmpx = cmplx((Real)re,(Real)im);

	  pokeColor(tmp_vec, 
		    tmp_cmpx,
		    col);
	}
	pokeSpin(q_tmp, 
		 tmp_vec, 
		 spin);
      }

      pokeSite(q, q_tmp, coord);

      //cout << q_tmp << endl;
      //read(cfg_in, q, coord); 	// Read in a site propagator
    }
  }

  cfg_in.close();
}
