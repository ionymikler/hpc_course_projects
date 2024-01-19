#!/bin/sh 
### General options 
### -- specify queue -- 
#BSUB -q hpcintrogpu

### -- set the job Name --
#BSUB -J batchjob

### -- ask for number of cores (default: 1) -- 
#BSUB -n 32

#BSUB -gpu "num=1:mode=exclusive_process"
#BSUB -R "span[hosts=1]"

### -- specify memory need per core/slot -- 
#BSUB -R "rusage[mem=2048]"

### -- set walltime limit: hh:mm -- 
#BSUB -W 40

### -- send notification at start -- 
#BSUB -B

### -- send notification at completion -- 
#BSUB -N

### -- Specify the output and error file. %J is the job-id -- 
### -- -o and -e mean append, -oo and -eo mean overwrite -- 
##BSUB -o Output_%J.out
##BSUB -e Output_%J.err 

module load /appl9/nvhpc/2023_2311/modulefiles/nvhpc-nompi/23.11


Ns="10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95 100"


rm -f "results_thread_j_p.txt"
rm -f "results.txt"

iterations=5000
startT=0
tolerance=0.1
threds=8

# === Warm up device
warmUpDevice

# === Compare data transfering strategies

# for N in $Ns
# do
#     OMP_NUM_THREADS=$threds ./Poisson_baseline/poisson_j $N $iterations $tolerance $startT $threds 0 0 # cpu parralize
#     ./Poisson_map/poisson $N $iterations $startT
#     # ./Poisson_memcpy/poisson $N $iterations $startT
# done


# === compare gpu and cpu parallization with stopping criteria

for N in $Ns
do
    OMP_NUM_THREADS=$threds ./Poisson_baseline_ES/poisson_j $N $iterations $tolerance $startT $threds 0 0
    ./Poisson_map_ES/poisson $N $iterations $startT $tolerance
done

# === compare 1 gpu with 2 gpus



exit 0
