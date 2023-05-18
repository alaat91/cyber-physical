#!/usr/local/bin/gnuplot
set datafile separator ','
set terminal pngcairo font 'sans'
set title 'performence test'
set ylabel 'groundSteering'
set xlabel 'timestamps in milliseconds'
set format x "%.0s*10^{%T}"
set output "performance_test.png"
plot 'data.csv' using 4:1 w l title 'ourGSR' , '' using 4:3 w l title "actualGSR"