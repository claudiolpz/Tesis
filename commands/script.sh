#!/bin/bash
#chmod +x script.sh
files=(
        # 'benchs/optim/hard/chem.bch'
        # 'benchs/optim/hard/chem-1.bch'
        # 'benchs/optim/hard/chembis.bch'
        # 'benchs/optim/hard/ex6_1_3.bch'
        # 'benchs/optim/hard/ex6_2_10.bch'
        # 'benchs/optim/hard/ex8_5_1bis.bch'
        # 'benchs/optim/hard/sambal.bch'
        'benchs/optim/medium/ex6_2_6.bch'
        'benchs/optim/medium/ex6_2_8.bch'
        'benchs/optim/medium/ex6_2_9.bch'
        'benchs/optim/medium/ex6_2_12.bch'
        'benchs/optim/medium/ex8_4_4bis.bch'
        'benchs/optim/medium/ex14_2_7.bch'
        'benchs/optim/medium/ex6_1_3bis.bch'
        'benchs/optim/medium/ex2_1_9.bch'
        'benchs/optim/medium/himmel16.bch'
        'benchs/optim/medium/ex2_1_7.bch'
        'benchs/optim/medium/ex2_1_8.bch'
        'benchs/optim/medium/launch.bch'
        # 'benchs/optim/coconutbenchmark-library2/hs088.nl'
        # 'benchs/optim/coconutbenchmark-library2/hs093.nl'
        # 'benchs/optim/coconutbenchmark-library2/hs102.nl'
        # 'benchs/optim/coconutbenchmark-library2/hs103.nl'
        # 'benchs/optim/coconutbenchmark-library2/hs108.nl'
        # 'benchs/optim/coconutbenchmark-library2/hs113.nl'
        # 'benchs/optim/coconutbenchmark-library2/dualc5.nl'
        # 'benchs/optim/coconutbenchmark-library2/dualc8.nl'
        # 'benchs/optim/coconutbenchmark-library2/mistake.nl'
        # 'benchs/optim/coconutbenchmark-library2/odfits.nl'
        # 'benchs/optim/blowup/ex2_1_9bis.nl'
      )

for file in \$${files[@]}; do
        for i in {1..5}; do
          ./__build__/src/ibexopt \$$file --random-seed=\$$i >> simulated-annealing.log
        done
      done