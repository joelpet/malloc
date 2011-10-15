set xlabel "Iterations"
set ylabel "Time (s)"
set terminal png
set output "tstbestcase_memory.png"

plot  "tstbestcase.0.dat" using 1:2 with linespoints title "System",\
      "tstbestcase.1.dat" using 1:2 with linespoints title "First fit",\
      "tstbestcase.2.dat" using 1:2 with linespoints title "Best",\
      "tstbestcase.3.dat" using 1:2 with linespoints title "Worst",\
      "tstbestcase.4.dat" using 1:2 with linespoints title "Quick"
