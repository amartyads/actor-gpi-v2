#!/bin/bash
#SBATCH -J UPCweak2
#SBATCH -o ./%x.%j.%N.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2_tiny
#SBATCH --partition=cm2_tiny
# no qos specification flag needed
#SBATCH --nodes=2
#SBATCH --tasks-per-node=28
#SBATCH --mail-type=all
#SBATCH --mail-user=ga53qud@mytum.de
#SBATCH --export=NONE
#SBATCH --time=00:30:00
  
module load slurm_setup

module load metis/5.1.0-intel19-i32-r32
module load netcdf-hdf5-all/4.6_hdf5-1.10-intel19-impi
module load gcc/8
module load amplifier_xe

export PATH="/dss/dsshome1/lxc06/ga53qud2/upc_new/bin:$PATH"

cd /dss/dsshome1/lxc06/ga53qud2/actor-upcxx/build

export OMP_NUM_THREADS=1

upcxx-run -n 56 amplxe-cl --collect hotspots -r /dss/dsshome1/lxc06/ga53qud2/actor-gpi-v2/amplifier-analysis/UPCweak2/ampli ./pond -x 4096 -y 4096 -p 256 -e 1 -c 1 --scenario 2 -o output/out
