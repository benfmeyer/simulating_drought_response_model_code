#!/bin/bash
#SBATCH -J GUESS_NCPS
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
#SBATCH --time=05:00:00

module load slurm_setup


export PATH=/dss/dsshome1/lrz/sys/spack/staging/20.1.1/opt/haswell/cdo/1.9.7.1-gcc-3v36tjp/bin/:$PATH
export PATH=/dss/dsshome1/lrz/sys/spack/staging/20.1.1/opt/haswell/nco/4.7.9-intel-v5h4o26/bin/:$PATH


scenario=$1
variable=$2

echo "preparing $scenario $variable"

cd ${scenario}_$variable

cdo -O -z zip_1 mergetime ${scenario}_${variable}_*.nc ${scenario}_${variable}.nc

if [ "$variable" = "swd" ]
then
        ncatted -a standard_name,swd,o,c,"surface_downwelling_shortwave_flux" ${scenario}_swd.nc ${scenario}_swd_lpjtemp.nc
        ncatted -a units,swd,o,c,"W m-2" ${scenario}_swd_lpjtemp.nc ${scenario}_swd_lpj.nc
        rm ${scenario}_swd_lpjtemp.nc
elif [ "$variable" = "tas" ] || [ "$variable" = "tasmin" ] || [ "$variable" = "tasmax" ]
then
        ncatted -a standard_name,$variable,o,c,"air_temperature" ${scenario}_${variable}.nc ${scenario}_${variable}_lpj.nc
elif [ "$variable" = "pr" ]
then
        ncatted -a standard_name,pr,o,c,"precipitation_flux" ${scenario}_pr.nc ${scenario}_pr_lpjtemp.nc
        ncatted -a units,pr,o,c,"kg m-2 s-1" ${scenario}_pr_lpjtemp.nc ${scenario}_pr_lpj.nc
        rm ${scenario}_pr_lpjtemp.nc
fi
