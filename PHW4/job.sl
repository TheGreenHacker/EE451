#!/bin/bash
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=4
#SBATCH --time=00:01:00
#SBATCH --output=mpijob.out
#SBATCH --error=mpijob.err

# mpirun -n $SLURM_NTASKS ./p1
# mpirun -n $SLURM_NTASKS ./p2_1
# mpirun -n $SLURM_NTASKS ./p2_2
mpirun -n $SLURM_NTASKS ./p2_3
# mpirun -n $SLURM_NTASKS ./all_to_one_reduction