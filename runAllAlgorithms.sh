#!/bin/bash
#SBATCH --time=00:20:00
#SBATCH --account=pn72pu
#SBATCH --partition=test
#SBATCH --open-mode=append
#SBATCH -o /dss/dsshome1/00/di93tuh/parallel-dense-mst/output.txt
#SBATCH --ntasks=100
module load slurm_setup

mpirun -np $SLURM_NTASKS ./build/MST algo=kruskal p=17 n=20 t