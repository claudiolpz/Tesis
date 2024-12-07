#include <vector>
#include "ibex_IntervalVector.h"
#include "ibex_System.h"
#include <iostream> 

namespace ibex {
    class SA_PSO {
        public:
            SA_PSO(const IntervalVector& box, const System& sys, int swarm_size, int max_iter, 
                double temperature, double cooling_rate, double min_temperature);

            double optimize();
            IntervalVector gbest_position;

        private:
            void initialize_particles();
            double objective_function(const IntervalVector& position);
            void update_particles();
            double acceptance_probability(double old_value, double new_value, double temperature);

            const IntervalVector& box;
            const System& sys;
            int swarm_size;
            int max_iter;
            double temperature;
            double cooling_rate;
            double min_temperature;
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


