#!/bin/bash
#SBATCH -J GPIstrong8
#SBATCH -o ./%x.%j.%N.out
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2
#SBATCH --partition=cm2_std
# no qos specification flag needed
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=10
#SBATCH --exclude=i22r06c05s[09-10],i22r02c05s12,i22r03c04s04,i22r02c05s10,i22r02c01s05,i22r04c01s05,i22r04c01s06
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

cd /dss/dsshome1/lxc06/ga53qud2/actor-gpi-v2/build

export OMP_NUM_THREADS=1

gaspi_logger &
mpiexec -n 80 --perhost 10 ./pond -x 16384 -y 16384 -p 512 -e 1 -c 1 --scenario 2 -o output/out
