for ((i=1; i<=7; i++))
do
	time ./task2 <$i.in >$i.ans
	diff -b -B $i.ans $i.out
done
