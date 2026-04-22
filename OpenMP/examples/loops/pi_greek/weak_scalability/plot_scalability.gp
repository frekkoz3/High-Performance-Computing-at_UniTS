# Accept the TYPE identifier from the command line (ARG1)
if (strlen(ARG1) == 0) {
    print "Error: Please provide the TYPE identifier as an argument."
    print "Usage: gnuplot -c plot_scalability.gnu <TYPE>"
    print "Example: gnuplot -c plot_scalability.gnu v2"
    exit
}

type_str = ARG1
data_file = sprintf("scalability_data.%s.txt", type_str)

# -----------------------------------------------------
# PLOT 1: Absolute Execution Time
# -----------------------------------------------------
out_file_time = sprintf("scalability.%s.png", type_str)

set terminal pngcairo size 800,600 enhanced font 'Arial,12'
set output out_file_time

set title sprintf("OpenMP Scalability: Execution Time vs Threads (%s)", type_str)
set xlabel "Number of Threads"
set ylabel "Execution Time (seconds)"

set grid
set key top right
set style fill transparent solid 0.3 noborder

plot data_file using 1:6:7 with filledcurves title '± 1 Std Dev', \
     data_file using 1:4 with linespoints linewidth 2 pointtype 7 pointsize 1.2 linecolor rgb "blue" title 'Average Time'

# -----------------------------------------------------
# PLOT 2: Weak Scaling speedup
# -----------------------------------------------------
out_file_eff = sprintf("speedup.%s.png", type_str)

# Extract the average time of the first data row (T_1)
# 'every ::0::0' tells Gnuplot to only look at the very first row of data
stats data_file using 4 every ::0::0 nooutput name "T_BASE"

# Since we only read one row, the "min" value is exactly our base time
baseline = T_BASE_min

set output out_file_eff

set title sprintf("OpenMP Weak Scaling speedup (%s)", type_str)
set ylabel "Normalized Execution Time (T_N / T_1)"

# In ideal weak scaling, the time should stay perfectly constant at 1.0.
# We plot the normalized time alongside a baseline reference of 1.
plot data_file using 1:($4 / baseline) with linespoints linewidth 2 pointtype 7 pointsize 1.2 linecolor rgb "red" title 'Normalized Avg Time', \
     1 with lines dashtype 2 linewidth 2 linecolor rgb "black" title 'Ideal speedup (1.0)'


# -----------------------------------------------------
# PLOT 3: Weak Scaling Efficiency
# -----------------------------------------------------
out_file_eff = sprintf("efficiency.%s.png", type_str)

# Extract the average time of the first data row (T_1)
# 'every ::0::0' tells Gnuplot to only look at the very first row of data
stats data_file using 4 every ::0::0 nooutput name "T_BASE"

# Since we only read one row, the "min" value is exactly our base time
baseline = T_BASE_min

set output out_file_eff

set title sprintf("OpenMP Weak Scaling Efficiency (%s)", type_str)
set ylabel "Normalized Execution Time (T_1 / T_N)"

# In ideal weak scaling, the time should stay perfectly constant at 1.0.
# We plot the normalized time alongside a baseline reference of 1.
plot data_file using 1:(baseline/$4) with linespoints linewidth 2 pointtype 7 pointsize 1.2 linecolor rgb "red" title 'Normalized Avg Time', \
     1 with lines dashtype 2 linewidth 2 linecolor rgb "black" title 'Ideal Efficiency (1.0)'
