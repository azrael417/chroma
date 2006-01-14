// -*- C++ -*-
// $Id: two_flavor_monomial_w.h,v 2.3 2006-01-14 05:56:59 edwards Exp $

/*! @file
 * @brief Two flavor Monomials - gauge action or fermion binlinear contributions for HMC
 */

#ifndef __two_flavor_monomial_w_h__
#define __two_flavor_monomial_w_h__

#include "update/molecdyn/monomial/abs_monomial.h"
#include "update/molecdyn/predictor/chrono_predictor.h"
#include "actions/ferm/invert/invert.h"
#include "invtype.h"

namespace Chroma
{
  //-------------------------------------------------------------------------------------------
  //! Exact 2 degen flavor fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor fermact monomial. Preconditioning is not
   * specified yet.
   * Can supply a default dsdq and pseudoferm refresh algorithm
   * 
   * CAVEAT: I assume there is only 1 pseudofermion field in the following
   * so called TwoFlavorExact monomial.
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactWilsonTypeFermMonomial : public ExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactWilsonTypeFermMonomial() {}

    //! Compute the total action
    virtual Double S(const AbsFieldState<P,Q>& s) = 0;

    //! Compute dsdq for the system... 
    /*! Monomial of the form  chi^dag*(M^dag*M)*chi */
    virtual void dsdq(P& F, const AbsFieldState<P,Q>& s)
    {
      // Self Description/Encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactWilsonTypeFermMonomial");

      /**** Identical code for unprec and even-odd prec case *****/
      
      // S_f  chi^dag*(M^dag*M)^(-1)*chi     
      // Here, M is some operator.
      //
      // Need
      // dS_f/dU = - psi^dag * [d(M^dag)*M + M^dag*dM] * psi,  psi = (M^dag*M)^(-1)*chi
      //
      // In Balint's notation, the result is  
      // \dot{S} = -X^dag*\dot{M}^\dag*Y - Y^dag\dot{M}*X
      // where
      //    X = (M^dag*M)^(-1)*chi   Y = M*X = (M^dag)^(-1)*chi
      // In Robert's notation,  X -> psi .
      //
      const WilsonTypeFermAct<Phi,P>& FA = getFermAct();
      
      // Create a state for linop
      Handle< const ConnectState> state(FA.createState(s.getQ()));
	
      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< const DiffLinearOperator<Phi,P> > lin(FA.linOp(state));
	
      Phi X, Y;

      // Get X out here
      // (getMDSolutionPredictor())(X);
      int n_count = getX(X,s);
      // (getMDSolutionPredictor()).newVector(X);
      
      (*lin)(Y, X, PLUS);

      lin->deriv(F, X, Y, MINUS);
      
      // fold M^dag into X^dag ->  Y  !!
      P F_tmp;
      lin->deriv(F_tmp, Y, X, PLUS);
      F += F_tmp;
 
      for(int mu=0; mu < F.size(); ++mu)
	F[mu] *= Real(-1);


      // F now holds derivative with respect to possibly fat links
      // now derive it with respect to the thin links if needs be
      state->deriv(F);

      Double F_sq = norm2(F);

      write(xml_out, "n_count", n_count);
      write(xml_out, "F_sq", F_sq);
      pop(xml_out);
    }
 
    //! Refresh pseudofermions
    virtual void refreshInternalFields(const AbsFieldState<P,Q>& field_state) 
    {
      // Heatbath all the fields
      
      // Get at the ferion action for piece i
      const WilsonTypeFermAct<Phi,P>& S_f = getFermAct();
      
      // Create a Connect State, apply fermionic boundaries
      Handle< const ConnectState > f_state(S_f.createState(field_state.getQ()));
      
      // Create a linear operator
      Handle< const LinearOperator<Phi> > M(S_f.linOp(f_state));
      
      Phi eta=zero;
      
      // Fill the eta field with gaussian noise
      gaussian(eta, M->subset());
      
      // Temporary: Move to correct normalisation
      eta *= sqrt(0.5);
      
      // Now HIT IT with the ROCK!!!! (Or in this case M^{dagger})
      (*M)(getPhi(), eta, MINUS);
      getMDSolutionPredictor().reset();
    }				    
  
    //! Copy pseudofermions if any
    virtual void setInternalFields(const Monomial<P,Q>& m) {
      try {
	const TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>& fm = dynamic_cast<  const TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>& >(m);

	getPhi() = fm.getPhi();
      }
      catch(bad_cast) { 
	QDPIO::cerr << "Failed to cast input Monomial to TwoFlavorExactWilsonTypeFermMonomial " << endl;
	QDP_abort(1);
      }

      // Resetting pseudofermion fields implies resetting the chrono predictor
      getMDSolutionPredictor().reset();
    }
  protected:
    //! Accessor for pseudofermion with Pf index i (read only)
    virtual const Phi& getPhi(void) const = 0;

    //! mutator for pseudofermion with Pf index i 
    virtual Phi& getPhi(void) = 0;    

    //! Get at fermion action
    virtual const WilsonTypeFermAct<Phi,P>& getFermAct(void) const = 0;

    //! Get inverter params
    virtual const InvertParam_t getInvParams(void) const = 0;

    //! Get the initial guess predictor
    virtual AbsChronologicalPredictor4D<Phi>& getMDSolutionPredictor(void) = 0;

    // Get X = (A^dag*A)^{-1} eta
    virtual int invert(Phi& X, const LinearOperator<Phi>& M, const Phi& eta) const
    {
      const InvertParam_t& inv_param = getInvParams();

      int n_count = 0;

      // Do the inversion...
      switch( inv_param.invType) {
      case CG_INVERTER:
      {
	// Solve MdagM X = eta
	InvCG2(M, eta, X, inv_param.RsdCG, inv_param.MaxCG, n_count);
	QDPIO::cout << "2Flav::invert,  n_count = " << n_count << endl;
      }
      break;
      default:
      {
	QDPIO::cerr << "Currently only CG Inverter is implemented" << endl;
	QDP_abort(1);
      }
      break;
      };
      
      return n_count;
    }


    //! Get (M^dagM)^{-1} phi
    virtual int getX(Phi& X, const AbsFieldState<P,Q>& s)
    {
      const InvertParam_t& inv_param = getInvParams();

      // Upcast the fermact
      const FermAct4D<Phi>& FA = getFermAct();

      // Make the state
      Handle< const ConnectState > state(FA.createState(s.getQ()));

      // Guess is passed in
   
      // Get linop
      Handle< const LinearOperator<Phi> > M(FA.linOp(state));
      int n_count;

      // Do the inversion...
      switch( inv_param.invType) {
      case CG_INVERTER:
      {
	// Solve MdagM X = eta
	// Do the inversion...
	  
	// Need MdagM for CG based predictor
	Handle< const LinearOperator<Phi> > MdagM(FA.lMdagM(state));
	(getMDSolutionPredictor())(X, *MdagM, getPhi());
	n_count = invert(X, *M, getPhi());
	(getMDSolutionPredictor()).newVector(X);
      }
      break;
      default:
      {
	QDPIO::cerr << "Currently only CG Inverter is implemented" << endl;
	QDP_abort(1);
      }
      break;
      };

      return n_count;
    }

  };


  //-------------------------------------------------------------------------------------------
  //! Exact 2 degen flavor unpreconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor unpreconditioned fermact monomial.
   * 
   * CAVEAT: I assume there is only 1 pseudofermion field in the following
   * so called TwoFlavorExact monomial.
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactUnprecWilsonTypeFermMonomial : public TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactUnprecWilsonTypeFermMonomial() {}

    //! Compute the total action
    virtual Double S(const AbsFieldState<P,Q>& s)
    {
      // Self identification/encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactUnprecWilsonTypeFermMonomial");

      Phi X;
      
      // Energy calc doesnt use Chrono Predictor
      X = zero;

      (getMDSolutionPredictor()).reset();
      int n_count = getX(X,s);

      // Action on the entire lattice
      Double action = innerProductReal(getPhi(), X);
      
      write(xml_out, "n_count", n_count);
      write(xml_out, "S", action);
      pop(xml_out);

      return action;
    }


  protected:
    //! Accessor for pseudofermion with Pf index i (read only)
    virtual const Phi& getPhi(void) const = 0;

    //! mutator for pseudofermion with Pf index i 
    virtual Phi& getPhi(void) = 0;    

    //! Get at fermion action
    virtual const UnprecWilsonTypeFermAct<Phi,P>& getFermAct(void) const = 0;

    //! Get inverter params
    virtual const InvertParam_t getInvParams(void) const = 0;

    //! Get at the chronological predcitor
    virtual AbsChronologicalPredictor4D<Phi>& getMDSolutionPredictor(void) = 0;
  };


  //-------------------------------------------------------------------------------------------
  //! Exact 2 degen flavor even-odd preconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor even-odd preconditioned fermact monomial.
   * Can supply a default dsdq algorithm
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial : public TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial() {}

    //! Even even contribution (eg ln det Clover)
    virtual Double S_even_even(const AbsFieldState<P,Q>& s)  = 0;

    //! Compute the odd odd contribution (eg
    virtual Double S_odd_odd(const AbsFieldState<P,Q>& s)
    {
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "S_odd_odd");

      const EvenOddPrecWilsonTypeFermAct<Phi,P>& FA = getFermAct();

      Handle<const ConnectState> bc_g_state = FA.createState(s.getQ());

      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< const EvenOddPrecLinearOperator<Phi,P> > lin(FA.linOp(bc_g_state));
      // Get the X fields
      Phi X;

      // Action calc doesnt use chrono predictor use zero guess
      X[ lin->subset() ] = zero;

      // getX noe always uses chrono predictor. Best to Nuke it therefore
      (getMDSolutionPredictor()).reset();
      int n_count = getX(X, s);
      Double action = innerProductReal(getPhi(), X, lin->subset());
      
      write(xml_out, "n_count", n_count);
      write(xml_out, "S_oo", action);
      pop(xml_out);

      return action;
    }

    //! Compute the total action
    Double S(const AbsFieldState<P,Q>& s)  {
      XMLWriter& xml_out=TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial");

      Double action = S_even_even(s) + S_odd_odd(s);

      write(xml_out, "S", action);
      pop(xml_out);
      return action;

    }

  protected:
    //! Get at fermion action
    virtual const EvenOddPrecWilsonTypeFermAct<Phi,P>& getFermAct() const = 0;

    //! Get inverter params
    virtual const InvertParam_t getInvParams(void) const = 0;

    //! Accessor for pseudofermion with Pf index i (read only)
    virtual const Phi& getPhi(void) const = 0;

    //! mutator for pseudofermion with Pf index i 
    virtual Phi& getPhi(void) = 0;    

    virtual AbsChronologicalPredictor4D<Phi>& getMDSolutionPredictor(void) = 0;
  };

}


#endif
