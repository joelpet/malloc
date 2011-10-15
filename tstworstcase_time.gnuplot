set xlabel "Iterations"
set ylabel "Time (s)"
set terminal png
set output "tstworstcase_time.png"

plot  "tstworstcase.0.dat" using 1:2 with linespoints title "System",\
      "tstworstcase.1.dat" using 1:2 with linespoints title "First fit",\
      "tstworstcase.2.dat" using 1:2 with linespoints title "Best",\
      "tstworstcase.3.dat" using 1:2 with linespoints title "Worst",\
      "tstworstcase.4.dat" using 1:2 with linespoints title "Quick"
