#!/bin/bash
#SBATCH -J UPCstrong8
#SBATCH -o ./%x.%j.%N.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2
#SBATCH --partition=cm2_std
# no qos specification flag needed
#SBATCH --nodes=8
#SBATCH --tasks-per-node=10
#SBATCH --exclude=i22r06c05s[09-10],i22r05c03s01,i22r04c02s01,i22r06c04s03,i22r04c01s12
#SBATCH --mail-type=all
#SBATCH --mail-user=ga53qud@mytum.de
#SBATCH --export=NONE
#SBATCH --time=03:00:00
i22r06c04s03,i22r04c01s12
module load slurm_setup

module load metis/5.1.0-intel19-i32-r32
module load netcdf-hdf5-all/4.6_hdf5-1.10-intel19-impi
module load gcc/8

export PATH="/dss/dsshome1/lxc06/ga53qud2/upc_new/bin:$PATH"

cd /dss/dsshome1/lxc06/ga53qud2/actor-upcxx/build

export OMP_NUM_THREADS=1

upcxx-run -n 80 ./pond -x 16384 -y 16384 -p 512 -e 1 -c 1 --scenario 2 -o output/out
