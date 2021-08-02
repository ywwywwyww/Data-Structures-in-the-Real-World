make
for ((i=1; i<=7; i++))
do
	time ./main <$i.in >$i.ans
	if diff -b -B $i.ans $i.out
	then
		echo 'ok'
	else
		echo 'wrong'
	fi
done
