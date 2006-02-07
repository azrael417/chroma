// -*- C++ -*-
// $Id: ks_local_loops.h,v 2.2 2006-02-07 11:54:49 egregory Exp $
/*! \file
 *  \brief Wrapper routines for computing loops with staggeref fermions
 */        


//
//
//

#ifndef  KS_LOCAL_LOOPS_INC 
#define  KS_LOCAL_LOOPS_INC 

#include "enum_loops_s.h"

namespace Chroma {

void ks_local_loops(
		 Handle<const SystemSolver<LatticeStaggeredFermion> > & qprop,
		 LatticeStaggeredFermion & q_source, 
		 LatticeStaggeredFermion & psi ,
		 const multi1d<LatticeColorMatrix> & u,
		 XMLWriter & xml_out, 
		 bool gauge_shift,
		 bool sym_shift,
		 bool loop_checkpoint,
		 int t_length,
		 Real Mass,
		 int Nsamp,
		 Real RsdCG,
		 int CFGNO,
		 VolSrc_type volume_source,
		 int src_seperation,
		 int j_decay) ;



void ks_local_loops(
		 Handle<const SystemSolver<LatticeStaggeredFermion> > & qprop,
		 LatticeStaggeredFermion & q_source, 
		 LatticeStaggeredFermion & psi ,
		 const multi1d<LatticeColorMatrix> & u,
		 XMLWriter & xml_out, 
		 XMLReader & xml_in ,
		 int t_length,
		 Real Mass,
		 int Nsamp,
		 Real RsdCG,
		 int CFGNO,
		 VolSrc_type volume_source,
		 int src_seperation,
		 int j_decay);



void ks_fuzz_loops(
		 Handle<const SystemSolver<LatticeStaggeredFermion> > & qprop,
		 LatticeStaggeredFermion & q_source, 
		 LatticeStaggeredFermion & psi ,
		 LatticeStaggeredFermion & psi_fuzz,
		 const multi1d<LatticeColorMatrix> & u,
		 const multi1d<LatticeColorMatrix> & u_smr,
		 XMLWriter & xml_out, 
		 bool gauge_shift,
		 bool sym_shift,
		 bool loop_checkpoint,
		 int t_length,
		 Real Mass,
		 int Nsamp,
		 Real RsdCG,
		 int CFGNO,
		 VolSrc_type volume_source,
		 int fuzz_width, 
		 int src_seperation,
		 int j_decay
		 )  ;



}  // end namespace Chroma

#endif
