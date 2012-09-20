//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_DMRGOBSERVER_H
#define __ITENSOR_DMRGOBSERVER_H
#include "observer.h"

//
// Class for monitoring DMRG calculations.
// The measure and checkDone methods are virtual
// so that behavior can be customized in a
// derived class.
//

class DMRGObserver : public Observer
    {
    public:
    
    DMRGObserver();

    virtual ~DMRGObserver() { }

    void virtual
    measure(int sw, int ha, int b, const SVDWorker& svd, Real energy,
              const Option& opt1 = Option(), const Option& opt2 = Option(), 
              const Option& opt3 = Option(), const Option& opt4 = Option());
    
    bool virtual
    checkDone(int sw, const SVDWorker& svd, Real energy,
                const Option& opt1 = Option(), const Option& opt2 = Option());

    Real 
    energyErrgoal() const { return energy_errgoal; }
    void 
    energyErrgoal(Real val) { energy_errgoal = val; }
    
    Real 
    orthWeight() const { return orth_weight; }
    void 
    orthWeight(Real val) { orth_weight = val; }
    
    bool 
    printEigs() const { return printeigs; }
    void 
    printEigs(bool val) { printeigs = val; }
    
    private:

    /////////////
    //
    // Data Members

    Vector center_eigs;
    Real energy_errgoal; //Stop DMRG once energy has converged to this precision
    Real orth_weight;    //How much to penalize non-orthogonality in multiple-state DMRG
    bool printeigs;      //Print slowest decaying eigenvalues after every sweep

    //
    /////////////

    }; // class DMRGObserver

inline DMRGObserver::
DMRGObserver() 
    : energy_errgoal(-1), 
      orth_weight(1),
      printeigs(true)
    { }


void inline DMRGObserver::
measure(int sw, int ha, int b, const SVDWorker& svd, Real energy,
        const Option& opt1, const Option& opt2, const Option& opt3, const Option& opt4)
    {
    if(printeigs)
        {
        if(b == 1 && ha == 2) 
            {
            std::cout << "\n    Largest m during sweep " << sw << " was " << svd.maxEigsKept() << "\n";
            std::cout << "    Largest truncation error: " << svd.maxTruncerr() << std::endl;
            Vector center_eigs = svd.eigsKept(svd.NN()/2);
            std::cout << "    Eigs at center bond: ";
            for(int j = 1; j <= min(center_eigs.Length(),10); ++j) 
                {
                std::cout << boost::format(center_eigs(j) > 1E-2 ? ("%.2f") : ("%.2E")) % center_eigs(j);
                std::cout << ((j != min(center_eigs.Length(),10)) ? ", " : "");
                }
            std::cout << std::endl;
            std::cout << boost::format("    Energy after sweep %d is %f") % sw % energy << std::endl;
            }
        }
    }


bool inline DMRGObserver::
checkDone(int sw, const SVDWorker& svd, Real energy,
          const Option& opt1, const Option& opt2)
    {
    static Real last_energy;
    
    if(sw == 1) last_energy = 1000;
    if(energy_errgoal > 0 && sw%2 == 0)
        {
        Real dE = fabs(energy-last_energy);
        if(dE < energy_errgoal)
            {
            std::cout << boost::format("    Energy error goal met (dE = %E); returning after %d sweeps.\n") % dE % sw;
            return true;
            }
        }
    last_energy = energy;
    
    return false;
    }

#endif // __ITENSOR_DMRGOBSERVER_H