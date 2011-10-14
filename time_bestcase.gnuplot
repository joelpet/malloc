set xlabel "Iterations"
set ylabel "Time (s)"

set terminal png
set output "time_bestcase.png"

plot  "tstbestcase.0.dat" using 1:3 with lines title "System",\
      "tstbestcase.1.dat" using 1:3 with lines title "First fit",\
      "tstbestcase.2.dat" using 1:3 with lines title "Best",\
      "tstbestcase.3.dat" using 1:3 with lines title "Worst",\
      "tstbestcase.4.dat" using 1:3 with lines title "Quick"

