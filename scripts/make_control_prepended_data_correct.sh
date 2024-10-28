#!/bin/bash
#SBATCH -J correct_time
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
#SBATCH --time=02:00:00

module load slurm_setup

export PATH=/dss/dsshome1/lrz/sys/spack/staging/20.1.1/opt/haswell/cdo/1.9.7.1-gcc-3v36tjp/bin/

for runname in hot hotdry dry noextremes nocompound
do
        for variable in tas tasmin tasmax pr swd
        do
                echo "$runname $variable"
                cdo -z zip_1 -setreftime,1900-01-01,00:00:00,1day control_${runname}_${variable}_lpj.nc control_${runname}_${variable}_lpj.nc_timed_zip
        done
done
