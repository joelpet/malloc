set xlabel "Iterations"
set ylabel "Memory"
set terminal png
set output "tstworstcase_memory.png"

plot  "tstworstcase.0.dat" using 1:3 with linespoints title "System",\
      "tstworstcase.1.dat" using 1:3 with linespoints title "First fit",\
      "tstworstcase.2.dat" using 1:3 with linespoints title "Best",\
      "tstworstcase.3.dat" using 1:3 with linespoints title "Worst",\
      "tstworstcase.4.dat" using 1:3 with linespoints title "Quick"
