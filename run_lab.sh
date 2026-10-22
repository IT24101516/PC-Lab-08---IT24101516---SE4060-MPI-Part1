#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p build
for source in Exercise0/*.cpp Exercise2/*.cpp Exercise3/*.cpp Exercise5/*.cpp Exercise6/*.cpp Exercise7/*.cpp; do
  folder=${source%%/*}; name=$(basename "$source" .cpp)
  mpicxx -DOMPI_SKIP_MPICXX -std=c++17 -O2 -Wall -Wextra "$source" -o "build/${folder}_${name}"
done
# Set MPI_FLAGS="--oversubscribe" only for deliberate single-host oversubscription.
# For multiple nodes, use MPI_FLAGS="--hostfile /path/to/hosts" after configuring all nodes.
read -r -a flags <<< "${MPI_FLAGS:-}"
run() { mpirun "${flags[@]}" -np "$1" "build/$2"; }
for name in hello message1 message2; do run 4 "Exercise0_$name" > "Exercise0/${name}_output.txt"; done
run 4 Exercise5_message2_bsend > Exercise5/bsend_output.txt
set +e
timeout 10s mpirun "${flags[@]}" -np 4 build/Exercise5_mismatch > Exercise5/mismatch_output.txt 2>&1
status=$?
set -e
printf 'timeout exit status: %s (124 expected)\n' "$status" >> Exercise5/mismatch_output.txt
if [ "$status" -ne 124 ]; then echo 'Mismatch test did not time out as expected' >&2; exit 1; fi
printf 'program,processes,seconds,result,pi\n' > Exercise4/timings.csv
for count in ${PROCESS_COUNTS:-1 2 4}; do
  for repeat in 1 2 3 4 5; do
    for target in Exercise2_sum Exercise3_pi Exercise6_pi Exercise7_pi; do
      run "$count" "$target" >> Exercise4/timings.csv
    done
  done
done
{ date -u; uname -a; lscpu; mpirun --version; printf 'MPI_FLAGS=%s\n' "${MPI_FLAGS:-}"; } > Exercise4/environment.txt
echo 'Runs completed. Generate graphs with: python3 Exercise4/plot_results.py'
