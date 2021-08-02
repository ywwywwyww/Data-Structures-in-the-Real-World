make
for ((i=1; i<=7; i++))
do
	time ./main <$i.in >$i.ans
done
