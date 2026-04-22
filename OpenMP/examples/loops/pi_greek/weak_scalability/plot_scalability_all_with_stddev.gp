# Set output format and file
set terminal pngcairo size 800,600 enhanced font 'Arial,12'
set output 'scalability_normalized_all.png'

# Titles and Labels
set title "OpenMP Weak Scaling: Normalized Execution Time with ±1 Std Dev"
set xlabel "Number of Threads"
set ylabel "Normalized Execution Time (T_N / T_1)"

# Aesthetics
set grid front
set key top left box opaque
set style data linespoints

# Define distinct colors for each version so bands and lines match
c_v0  = "#e41a1c"   # Red
c_v0b = "#377eb8"   # Blue
c_v1  = "#4daf4a"   # Green
c_v2  = "#984ea3"   # Purple

# -----------------------------------------------------
# 1. Extract the baselines (T_1) for each version
# -----------------------------------------------------
stats 'scalability_data.v0.txt'  using 4 every ::0::0 nooutput name "V0"
stats 'scalability_data.v0b.txt' using 4 every ::0::0 nooutput name "V0B"
stats 'scalability_data.v1.txt'  using 4 every ::0::0 nooutput name "V1"
stats 'scalability_data.v2.txt'  using 4 every ::0::0 nooutput name "V2"

# -----------------------------------------------------
# 2. Plotting (Bands first, then lines, then ideal baseline)
# -----------------------------------------------------
# We use column 6 (avg - std_dev) and column 7 (avg + std_dev) normalized by T_1

plot \
    'scalability_data.v0.txt'  using 1:($6/V0_min):($7/V0_min)  with filledcurves fill transparent solid 0.15 noborder linecolor rgb c_v0 notitle, \
    'scalability_data.v0b.txt' using 1:($6/V0B_min):($7/V0B_min) with filledcurves fill transparent solid 0.15 noborder linecolor rgb c_v0b notitle, \
    'scalability_data.v1.txt'  using 1:($6/V1_min):($7/V1_min)  with filledcurves fill transparent solid 0.15 noborder linecolor rgb c_v1 notitle, \
    'scalability_data.v2.txt'  using 1:($6/V2_min):($7/V2_min)  with filledcurves fill transparent solid 0.15 noborder linecolor rgb c_v2 notitle, \
    \
    'scalability_data.v0.txt'  using 1:($4/V0_min)  linewidth 2 pointtype 5 pointsize 1.2 linecolor rgb c_v0 title 'v0 (Atomic/Critical)', \
    'scalability_data.v0b.txt' using 1:($4/V0B_min) linewidth 2 pointtype 7 pointsize 1.2 linecolor rgb c_v0b title 'v0b', \
    'scalability_data.v1.txt'  using 1:($4/V1_min)  linewidth 2 pointtype 9 pointsize 1.2 linecolor rgb c_v1 title 'v1', \
    'scalability_data.v2.txt'  using 1:($4/V2_min)  linewidth 2 pointtype 11 pointsize 1.2 linecolor rgb c_v2 title 'v2 (Local Accumulator)', \
    \
    1 with lines dashtype 2 linewidth 2 linecolor rgb "black" title 'Ideal (1.0)'
