#!/bin/bash
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=1
#SBATCH --time=00:01:00
#SBATCH --output=p2.out
#SBATCH --error=p2.err
#SBATCH --gres=gpu:1

./p1
# ./p2