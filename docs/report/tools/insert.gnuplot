set ylabel 'Time [ps]'
set xlabel 'Elements'
set terminal pdf
set output 'insert_outlayers.pdf'
set datafile separator ','
set pointsize 1
set format x '%.s%c
plot 'insert_outlayers.dat' notitle lt rgb 'blue'
