// -*- C++ -*-
// $Id: unprec_dwf_fermact_base_w.h,v 1.2 2003-11-20 05:43:41 edwards Exp $
/*! \file
 *  \brief Base class for unpreconditioned domain-wall fermion action
 */

#ifndef __unprec_dwf_fermact_base_w_h__
#define __unprec_dwf_fermact_base_w_h__

#include "fermact.h"

using namespace QDP;

//! Base class for unpreconditioned domain-wall fermion action
/*! \ingroup fermact
 *
 * Unprecondition domain-wall fermion action. The conventions used here
 * are specified in Phys.Rev.D63:094505,2001 (hep-lat/0005002).
 */

class UnprecDWFermActBase : public UnprecWilsonTypeFermAct<LatticeDWFermion>
{
public:
  //! Return the quark mass
  virtual Real quark_mass() const = 0;

  //! Produce a linear operator for this action but with quark mass 1
  virtual const LinearOperator<LatticeDWFermion>* linOpPV(const multi1d<LatticeColorMatrix>& u) const = 0;

  //! Redefine quark propagator routine for 4D fermions
  void qprop(LatticeFermion& psi, 
	     const multi1d<LatticeColorMatrix>& u, 
	     const LatticeFermion& chi, 
	     enum InvType invType,
	     const Real& RsdCG, 
	     int MaxCG, int& ncg_had) const;

  //! Compute dS_f/dU
  void dsdu(multi1d<LatticeColorMatrix>& result,
	    const multi1d<LatticeColorMatrix>& u,
	    const LatticeDWFermion& psi) const;
};

#endif
