#include <vector>
#include "ibex_IntervalVector.h"
#include "ibex_System.h"
#include <iostream> 

namespace ibex {
    class PSO {
    public:
        PSO(const IntervalVector& box, const System& sys, int swarm_size, int max_iter);
        
        double optimize();
        IntervalVector gbest_position;

    private:
        void initialize_particles();
        double objective_function(const IntervalVector& position);
        void update_particles();

        const IntervalVector& box;
        const System& sys;
        int swarm_size;
        int max_iter;
        double inertia;
        double cognitive;
        double social;
        double gbest_value;
        std::vector<IntervalVector> particles;
        std::vector<IntervalVector> velocities;
        std::vector<IntervalVector> pbest_positions;
        std::vector<double> pbest_values;
    };

} 


