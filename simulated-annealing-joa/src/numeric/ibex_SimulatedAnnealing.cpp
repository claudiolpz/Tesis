#include "ibex_SimulatedAnnealing.h"
#include "ibex_IntervalVector.h"
#include <vector>
#include <iomanip>
#include "ibex_Random.h"
#include <iostream>
namespace ibex {
    SimulatedAnnealing::SimulatedAnnealing(const IntervalVector& box, const System& sys):box(box), sys(sys){}

    double SimulatedAnnealing::f_obj(const IntervalVector& x, int ctr) const {
        return sys.f_ctrs[ctr].eval(x).mid();
    }

    Vector SimulatedAnnealing::fun_per(const Vector& X_B, const IntervalVector& box, int ctr) {
        double mejor_eval = std::numeric_limits<double>::infinity();
        Vector mejor_sol(X_B.size()), xq = X_B;

        for(int j = 0; j < X_B.size(); ++j) {
            // Generar perturbación
            double d_lb = box[j].lb() - X_B[j];
            double d_ub = box[j].ub() - X_B[j];
            double perturbacion = d_lb + (d_ub - d_lb) * RNG::rand();

            xq[j] = xq[j] + perturbacion;

            // Garantizar que xq[j] permanezca dentro de los límites
            if (xq[j] < box[j].lb()) {
                xq[j] = box[j].lb();
            }
            if (xq[j] > box[j].ub()) {
                xq[j] = box[j].ub();
            }

            double eval_xq = this->f_obj(IntervalVector(xq), ctr);  
            if(eval_xq < mejor_eval) {
                mejor_eval = eval_xq;
                mejor_sol = xq;
            }
        }

        return mejor_sol;
    }

    Vector SimulatedAnnealing::fun_per2(const Vector& X_B, const IntervalVector& box, int ctr) {
        double mejor_eval = std::numeric_limits<double>::infinity();
        Vector mejor_sol(X_B.size()), xq = X_B;
        
        for (int j = 0; j < X_B.size(); ++j) {
            // Generar perturbación
            double d_lb = box[j].lb() - X_B[j];
            double d_ub = box[j].ub() - X_B[j];
            double perturbacion = d_lb + (d_ub - d_lb) * RNG::rand();

            xq[j] = xq[j] + perturbacion;

            // Asegurarse de que xq[j] permanezca dentro de los límites
            xq[j] = std::max(xq[j], box[j].lb());
            xq[j] = std::min(xq[j], box[j].ub());

            double eval_xq = this->f_obj(IntervalVector(xq), ctr);
            if(eval_xq < mejor_eval) {
                mejor_eval = eval_xq;
                mejor_sol = xq;
            }
        }

        return mejor_sol;
    }

    Vector SimulatedAnnealing::fun_per3(const Vector& X_B, const IntervalVector& box) {
        double mejor_eval = std::numeric_limits<double>::infinity();
        Vector mejor_sol(X_B.size()), xq = X_B;
        int dim = box.size();

        int num_perturbaciones = RNG::rand() % dim + 1;  // Número aleatorio de elementos a perturbar

        for(int j = 0; j < num_perturbaciones; ++j) {
            int idx = RNG::rand() % dim; // Selecciona un índice aleatorio para perturbar

            // Aplica una perturbación variada
            double delta = (box[idx].ub() - box[idx].lb()) * RNG::rand() * 0.1; // 10% del rango como máximo
            if(RNG::rand() % 2) {
                xq[idx] += delta; // Suma el delta
            } else {
                xq[idx] -= delta; // Resta el delta
            }

            // Asegurarse de que la solución perturbada esté dentro de los límites
            xq[idx] = std::max(box[idx].lb(), std::min(xq[idx], box[idx].ub()));

            // Evalúa la nueva solución perturbada
            double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
            int restriccion = std::round(restriccion1);
            double eval_xq = this->f_obj(IntervalVector(xq), restriccion);
            if(eval_xq < mejor_eval) {
                mejor_eval = eval_xq;
                mejor_sol = xq;
            }
        }
        return mejor_sol;
    }

    Vector SimulatedAnnealing::v1(const IntervalVector& box) {
        IntervalVector inicial(box.size());

        double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
        int restriccion = std::round(restriccion1);

        for (int i = 0; i < box.size(); i++) {
            double random_num = RNG::rand(box[i].lb(), box[i].ub());
            inicial[i] = Interval(random_num, random_num);
        }
        
        double mejor = f_obj(inicial, restriccion);  

        double temperatura = 100.0;
        double alpha = 0.95;

        bool cambio = true;
        IntervalVector vecino = inicial;
        int iter = 0;
        while (cambio && iter < 50) {
            cambio = false;
            for (int i = 0; i < inicial.size(); i++) {
                double ruido = RNG::rand(0, 1);
                double magnitud = (box[i].ub() - box[i].lb()) * 0.1;
                ruido = ruido * magnitud;
                vecino[i] = Interval(inicial[i].lb() + ruido, inicial[i].ub() + ruido);
                
                double delta = f_obj(vecino, restriccion) - mejor;

                double randomReal = (double) rand() / (double) RAND_MAX; // Número aleatorio real entre [0,1]
                
                Interval expInterval = exp(-delta / temperatura);
                double expValue = expInterval.mid();  // Obtenemos el punto medio del intervalo.

                if (delta < 0 || randomReal < expValue) {
                    mejor = f_obj(vecino, restriccion);
                    inicial = vecino;
                    cambio = true;
                    break;
                } else {
                    vecino = inicial;
                }
            }
            // Geometrico
            temperatura = alpha * temperatura; 
            iter++;
        }

        return inicial.mid();
    }

Vector SimulatedAnnealing::v2(const IntervalVector& box) {
    IntervalVector inicial(box.size());
    double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
    int restriccion = std::round(restriccion1);

    for (int i = 0; i < box.size(); i++) {
        double random_num = RNG::rand(box[i].lb(), box[i].ub());
        inicial[i] = Interval(random_num, random_num);
    }

    double mejor = f_obj(inicial, restriccion);  
    double temperatura = 1.0;
    double alpha = 0.95;
    bool cambio = true;
    IntervalVector vecino = inicial;
    int iter = 0;

    while (cambio && iter < 50) {
        cambio = false;

        Vector vecino_vec = inicial.mid();  // Convertir IntervalVector a Vector
        vecino_vec = fun_per2(vecino_vec, box, restriccion); // Aplicar perturbación
        vecino = IntervalVector(vecino_vec); // Convertir Vector a IntervalVector

        double delta = f_obj(vecino, restriccion) - mejor;
        double randomReal = (double) rand() / (double) RAND_MAX;

        Interval expInterval = exp(-delta / temperatura);
        double expValue = expInterval.mid();

        if (delta < 0 || randomReal < expValue) {
            mejor = f_obj(vecino, restriccion);
            inicial = vecino;
            cambio = true;
        } else {
            vecino = inicial;
        }

        temperatura = alpha * temperatura; 
        iter++;
    }

    return inicial.mid();
}

Vector SimulatedAnnealing::v3(const IntervalVector& box) {
    IntervalVector inicial(box.size());
    double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
    int restriccion = std::round(restriccion1);

    for (int i = 0; i < box.size(); i++) {
        double random_num = RNG::rand(box[i].lb(), box[i].ub());
        inicial[i] = Interval(random_num, random_num);
    }

    double mejor = f_obj(inicial, restriccion);  
    double temperatura = 100;
    double alpha = 0.95;
    bool cambio = true;
    IntervalVector vecino = inicial;
    int iter = 0;

    while (cambio && iter < 50) {
        cambio = false;

        Vector vecino_vec = inicial.mid();  // Convertir IntervalVector a Vector
        vecino_vec = fun_per2(vecino_vec, box, restriccion); // Aplicar perturbación
        vecino = IntervalVector(vecino_vec); // Convertir Vector a IntervalVector

        double delta = f_obj(vecino, restriccion) - mejor;
        double randomReal = (double) rand() / (double) RAND_MAX;

        Interval expInterval = exp(-delta / temperatura);
        double expValue = expInterval.mid();

        if (delta < 0 || randomReal < expValue) {
            mejor = f_obj(vecino, restriccion);
            inicial = vecino;
            cambio = true;
        } else {
            vecino = inicial;
        }

        temperatura = temperatura * (std::exp(-alpha * iter)); 
        iter++;
    }

    return inicial.mid();
}

Vector SimulatedAnnealing::v4(const IntervalVector& box) {
    IntervalVector inicial(box.size());
    double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
    int restriccion = std::round(restriccion1);

    for (int i = 0; i < box.size(); i++) {
        double random_num = RNG::rand(box[i].lb(), box[i].ub());
        inicial[i] = Interval(random_num, random_num);
    }

    double mejor = f_obj(inicial, restriccion);  
    double temperatura = 1;
    double alpha = 0.95;
    double T_inicial = 1;
    bool cambio = true;
    IntervalVector vecino = inicial;
    int iter = 0;

    while (cambio && iter < 50) {
        cambio = false;

        Vector vecino_vec = inicial.mid();  // Convertir IntervalVector a Vector
        vecino_vec = fun_per2(vecino_vec, box, restriccion); // Aplicar perturbación
        vecino = IntervalVector(vecino_vec); // Convertir Vector a IntervalVector

        double delta = f_obj(vecino, restriccion) - mejor;
        double randomReal = (double) rand() / (double) RAND_MAX;

        Interval expInterval = exp(-delta / temperatura);
        double expValue = expInterval.mid();

        if (delta < 0 || randomReal < expValue) {
            mejor = f_obj(vecino, restriccion);
            inicial = vecino;
            cambio = true;
        } else {
            vecino = inicial;
        }

        temperatura = T_inicial * (std::log(alpha + iter)); 
        iter++;
    }

    return inicial.mid();
}

Vector SimulatedAnnealing::v5(const IntervalVector& box) {
    IntervalVector inicial(box.size());
    double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
    int restriccion = std::round(restriccion1);

    for (int i = 0; i < box.size(); i++) {
        double random_num = RNG::rand(box[i].lb(), box[i].ub());
        inicial[i] = Interval(random_num, random_num);
    }

    double mejor = f_obj(inicial, restriccion);  
    double temperatura = 1.0;
    double b = 2;
    double T_inicial = 1;
    bool cambio = true;
    IntervalVector vecino = inicial;
    int iter = 0;

    while (cambio && iter < 50) {
        cambio = false;

        Vector vecino_vec = inicial.mid();  // Convertir IntervalVector a Vector
        vecino_vec = fun_per2(vecino_vec, box, restriccion); // Aplicar perturbación
        vecino = IntervalVector(vecino_vec); // Convertir Vector a IntervalVector

        double delta = f_obj(vecino, restriccion) - mejor;
        double randomReal = (double) rand() / (double) RAND_MAX;

        Interval expInterval = exp(-delta / temperatura);
        double expValue = expInterval.mid(); // Obteniendo el punto medio del intervalo

        if (delta < 0 || randomReal < expValue) {
            mejor = f_obj(vecino, restriccion);
            inicial = vecino;
            cambio = true;
        } else {
            vecino = inicial;
        }
        temperatura = temperatura * (T_inicial / (std::pow(1 + iter, b))); 
        iter++;
    }

    return inicial.mid();
}

Vector SimulatedAnnealing::v6(const IntervalVector& box) {
    IntervalVector inicial(box.size());
    double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
    int restriccion = std::round(restriccion1);

    // Generar solución inicial
    for (int i = 0; i < box.size(); i++) {
        double random_num = RNG::rand(box[i].lb(), box[i].ub());
        inicial[i] = Interval(random_num, random_num);
    }

    double mejor_eval = f_obj(inicial, restriccion);  
    double temperatura = 1.0;
    double alpha = 0.95;
    IntervalVector vecino = inicial;
    int iter = 0;

    while (iter < 50) {
        // Aplicar la función de perturbación
        Vector vecino_vec = fun_per3(inicial.mid(), box);
        vecino = IntervalVector(vecino_vec); // Convertir Vector a IntervalVector

        double eval_vecino = f_obj(vecino, restriccion);
        double delta = eval_vecino - mejor_eval;
        double randomReal = (double) rand() / (double) RAND_MAX;

        Interval expInterval = exp(-delta / temperatura);
        double expValue = expInterval.mid(); // Obteniendo el punto medio del intervalo

        if (delta < 0 || randomReal < expValue) { // Comparando con el punto medio
            mejor_eval = eval_vecino;
            inicial = vecino;
        }

        // Enfriar la temperatura
        temperatura *= alpha;
        iter++;
    }

    return inicial.mid();
}
Vector SimulatedAnnealing::v7(const IntervalVector& box) {
    IntervalVector inicial(box.size());
    double restriccion1 = RNG::rand(0, this->sys.nb_ctr - 1); 
    int restriccion = std::round(restriccion1);

    for (int i = 0; i < box.size(); i++) {
        double random_num = RNG::rand(box[i].lb(), box[i].ub());
        inicial[i] = Interval(random_num, random_num);
    }

    double mejor = f_obj(inicial, restriccion);  
    double temperatura = 1.0;
    double alpha = 0.95;
    bool cambio = true;
    IntervalVector vecino = inicial;
    int iter = 0;

    while (cambio && iter < 500) {
        cambio = false;

        Vector vecino_vec = inicial.mid();  // Convertir IntervalVector a Vector
        vecino_vec = fun_per3(vecino_vec, box); // Aplicar perturbación
        vecino = IntervalVector(vecino_vec); // Convertir Vector a IntervalVector

        double delta = f_obj(vecino, restriccion) - mejor;
        double randomReal = (double) rand() / (double) RAND_MAX;

        Interval expInterval = exp(-delta / temperatura);
        double expValue = expInterval.mid();

        if (delta < 0 || randomReal < expValue) {
            mejor = f_obj(vecino, restriccion);
            inicial = vecino;
            cambio = true;
        } else {
            vecino = inicial;
        }

        temperatura = alpha * temperatura; 
        iter++;
    }

    return inicial.mid();
}
}

