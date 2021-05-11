set ylabel 'Time [ps]'
set xlabel 'Elements'
set terminal pdf
set output 'contains_outlayers.pdf'
set datafile separator ','
set pointsize 1
set format x '%.s%c
plot 'contains_outlayers.dat' notitle lt rgb 'blue'
