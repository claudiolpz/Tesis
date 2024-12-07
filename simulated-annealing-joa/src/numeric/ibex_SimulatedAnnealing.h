#include <vector>
#include "ibex_IntervalVector.h"
#include "ibex_System.h"
#include <iostream> 
namespace ibex {
    class SimulatedAnnealing{
        public:
            SimulatedAnnealing(const IntervalVector& box, const System& sys);
            double f_obj(const IntervalVector& x, int ctr) const;
            Vector fun_per(const Vector& X_B, const IntervalVector& box, int ctr);
            Vector fun_per2(const Vector& X_B, const IntervalVector& box, int ctr);
            Vector fun_per3(const Vector& X_B, const IntervalVector& box);
            Vector v1(const IntervalVector& box);
            Vector v2(const IntervalVector& box);
            Vector v3(const IntervalVector& box);
            Vector v4(const IntervalVector& box);
            Vector v5(const IntervalVector& box);
            Vector v6(const IntervalVector& box);
            Vector v7(const IntervalVector& box);
        private:
            const IntervalVector& box;
            const System& sys;
            
    };
}