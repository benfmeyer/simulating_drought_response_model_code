#!/bin/bash
#SBATCH -J PREPEND_CONTROL
#SBATCH --error=%j.err
#SBATCH --output=%j.log
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --clusters=cm2_tiny
#SBATCH --partition=cm2_tiny
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mail-type=ALL
#SBATCH --mail-user=konstantin.gregor@tum.de
#SBATCH --export=NONE
#SBATCH --time=00:20:00


# Note: for variable pr, need more time than that!


# Note: make sure cdo and nco is in your path:
export PATH=/dss/dsshome1/lrz/sys/spack/staging/20.1.1/opt/haswell/cdo/1.9.7.1-gcc-3v36tjp/bin/:$PATH
export PATH=/dss/dsshome1/lrz/sys/spack/staging/20.1.1/opt/haswell/nco/4.7.9-intel-v5h4o26/bin/:$PATH

module load slurm_setup

scenario=$1
variable=$2


echo "prepending control to $scenario $variable"
cdo -O -z zip_1 mergetime control_${variable}_lpj_shifted.nc ${scenario}_${variable}/${scenario}_${variable}_lpj.nc control_${scenario}_${variable}_lpj.nc
