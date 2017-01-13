#!/bin/bash
# 02614 - High-Performance Computing, January 2017
# 
# batch script to run collect on a decidated server in the hpcintro
# queue
#
# Author: Bernd Dammann <bd@cc.dtu.dk>
#
#PBS -N collector_poisson
#PBS -q hpcintro
#PBS -l nodes=1:ppn=20
#PBS -l walltime=10:00

cd $PBS_O_WORKDIR

module load studio

EXECUTABLE=poisson

# experiment name 
#
JID=`echo ${PBS_JOBID%.*}`
EXPOUT="$PBS_JOBNAME.${JID}.er"

# start the collect command with the above settings
lscpu
Increment=1
NUM_THREADS=1
N=120
FUNC="jac"
OMP_STRING="OMP_NUM_THREADS="
for I in {1..8}
do
    OMP_NUM_THREADS=$NUM_THREADS ./$EXECUTABLE $N $FUNC
    NUM_THREADS=$(($NUM_THREADS + $Increment))
done

