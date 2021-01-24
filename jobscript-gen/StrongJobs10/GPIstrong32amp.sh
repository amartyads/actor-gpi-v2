#!/bin/bash
#SBATCH -J GPIstrong32amp2
#SBATCH -o ./%x.%j.%N.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2
#SBATCH --partition=cm2_large
#SBATCH --qos=cm2_large
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=10
#SBATCH --exclude=i22r06c05s[09-10]
#SBATCH --mail-type=all
#SBATCH --mail-user=ga53qud@mytum.de
#SBATCH --export=NONE
#SBATCH --time=03:00:00
  
module load slurm_setup

module use -p /lrz/sys/spack/.tmp/test/gpi2/modules/x86_avx2/linux-sles12-x86_64
module load gpi-2/1.4.0-gcc8-impi-rdma-core-24
module load metis/5.1.0-intel19-i32-r32
module load netcdf-hdf5-all/4.6_hdf5-1.10-intel19-impi
module load gcc/8
module load amplifier_xe

cd /dss/dsshome1/lxc06/ga53qud2/actor-gpi-v2/build

export OMP_NUM_THREADS=1

gaspi_logger &
mpiexec -n 320 --perhost 10 amplxe-cl --collect hotspots -r /dss/dsshome1/lxc06/ga53qud2/actor-gpi-v2/amplifier-analysis/GPIstrong32datac/ampli ./pond -x 16384 -y 16384 -p 256 -e 1 -c 1 --scenario 2 -o output/out
