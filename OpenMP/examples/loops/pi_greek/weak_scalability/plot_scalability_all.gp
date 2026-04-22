# Set output format and file
set terminal pngcairo size 800,600 enhanced font 'Arial,12'
set output 'scalability_normalized_all.png'

# Titles and Labels
set title "OpenMP Weak Scaling: Normalized Execution Time Comparison"
set xlabel "Number of Threads"
set ylabel "Normalized Execution Time (T_N / T_1)"

# Aesthetics
set grid
set key top left box opaque
set style data linespoints

# -----------------------------------------------------
# 1. Extract the baselines (T_1) for each version
# -----------------------------------------------------
# 'every ::0::0' tells Gnuplot to only look at the very first row
stats 'scalability_data.v0.txt'  using 4 every ::0::0 nooutput name "V0"
stats 'scalability_data.v0b.txt' using 4 every ::0::0 nooutput name "V0B"
stats 'scalability_data.v1.txt'  using 4 every ::0::0 nooutput name "V1"
stats 'scalability_data.v2.txt'  using 4 every ::0::0 nooutput name "V2"

# -----------------------------------------------------
# 2. Plot all lines together
# -----------------------------------------------------
# We divide column 4 by the respective baseline for each file.
# The baseline is stored in the '_min' variable by the stats command.

plot 'scalability_data.v0.txt'  using 1:($4/V0_min)  linewidth 2 pointtype 5 pointsize 1.2 title 'v0 (Atomic/Critical)', \
     'scalability_data.v0b.txt' using 1:($4/V0B_min) linewidth 2 pointtype 7 pointsize 1.2 title 'v0b', \
     'scalability_data.v1.txt'  using 1:($4/V1_min)  linewidth 2 pointtype 9 pointsize 1.2 title 'v1', \
     'scalability_data.v2.txt'  using 1:($4/V2_min)  linewidth 2 pointtype 11 pointsize 1.2 title 'v2 (Local Accumulator)', \
     1 with lines dashtype 2 linewidth 2 linecolor rgb "black" title 'Ideal (1.0)'
