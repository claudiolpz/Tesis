#include "ibex_PSO.h"
#include "ibex_IntervalVector.h"
#include <vector>
#include "ibex_Random.h"
#include <cmath>
#include <limits>
#include <iostream>
#include <iomanip>

namespace ibex {
PSO::PSO(const IntervalVector& box, const System& sys, int swarm_size, int max_iter)
    : box(box), sys(sys), swarm_size(swarm_size), max_iter(max_iter),
      inertia(0.85), cognitive(2.5), social(2.5),
      gbest_value(std::numeric_limits<double>::max()) {}

void PSO::initialize_particles() {
    particles.resize(swarm_size);
    velocities.resize(swarm_size);
    pbest_positions.resize(swarm_size);
    pbest_values.resize(swarm_size, std::numeric_limits<double>::max());

    for (int i = 0; i < swarm_size; ++i) {
        particles[i] = IntervalVector(box.size());
        velocities[i] = IntervalVector(box.size());

        for (int j = 0; j < box.size(); ++j) {
            particles[i][j] = Interval(RNG::rand(box[j].lb(), box[j].ub()));
            velocities[i][j] = Interval(0);
        }

        pbest_positions[i] = particles[i];
        pbest_values[i] = objective_function(particles[i]);
        if (pbest_values[i] < gbest_value) {
            gbest_value = pbest_values[i];
            gbest_position = particles[i];
        }
    }
}


double PSO::objective_function(const IntervalVector& position) {
    double sum = 0;
    for (int i = 0; i < sys.f_ctrs.image_dim(); ++i) {
        sum += sys.f_ctrs[i].eval(position).mid();
    }
    return sum;
}


void PSO::update_particles() {
    for (int i = 0; i < swarm_size; ++i) {
        Vector r1(box.size());
        Vector r2(box.size());
        for (int j = 0; j < box.size(); ++j) {
            r1[j] = RNG::rand(0, 1);
            r2[j] = RNG::rand(0, 1);
        }

        for (int j = 0; j < box.size(); ++j) {
            velocities[i][j] = velocities[i][j] * inertia +
                               (pbest_positions[i][j] - particles[i][j]) * cognitive * r1[j] +
                               (gbest_position[j] - particles[i][j]) * social * r2[j];

            particles[i][j] += velocities[i][j];

            if (particles[i][j].lb() < box[j].lb() || particles[i][j].ub() > box[j].ub()) {
                velocities[i][j] = -velocities[i][j];
                particles[i][j] += velocities[i][j];
            }
        }

        double current_value = objective_function(particles[i]);
        if (current_value < pbest_values[i]) {
            pbest_values[i] = current_value;
            pbest_positions[i] = particles[i];
        }
        if (current_value < gbest_value) {
            gbest_value = current_value;
            gbest_position = particles[i];
        }
    }
}


double PSO::optimize() {
    initialize_particles();
    double prev_gbest_value = std::numeric_limits<double>::max();
    for (int iter = 0; iter < max_iter; ++iter) {
        update_particles();
        if (std::abs(gbest_value - prev_gbest_value) < 1e-6 && iter > 5) {
            break; 
        }
        prev_gbest_value = gbest_value;
    }
    return gbest_value;
}

} // namespace ibex
