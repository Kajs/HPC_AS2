#!/bin/bash
gnuplot -p <<- EOF
    set xlabel "xlabel"
    set ylabel "ylabel"
    set zlabel "zlabel"
    set title "Test 3d plot"
    set xrange[-1:1]
    set yrange[-1:1]

    set dgrid3d 30,30
    set hidden3d
    set palette defined (-10 "blue", -5 "cyan", 0 "grey", 5 "yellow", 10 "red")
    #set term png
    #set output "plot_nat_small.jpg"
    splot "data.dat" using 1:2:3 with pm3d
    
EOF
