# Data-Structures-in-the-Real-World
course 'Data Structures in the Real World' in freshman year, summer semester



### B Tree vs. std::set

$10^7$ 次查询

```shell
2020011000@w3:~/dsrw/other projects/b_tree_test$ pidof rbt_main
13543
2020011000@w3:~/dsrw/other projects/b_tree_test$ perf stat -p 13543 -e task-clo
ck,cycles,instructions,cache-references,cache-misses,branches,branch-misses

 Performance counter stats for process id '13543':

         10,199.38 msec task-clock                #    0.728 CPUs utilized
    42,160,120,598      cycles                    #    4.134 GHz      (83.32%)
     2,812,507,889      instructions              #    0.07  insn per cycle      (83.34%)
       681,355,392      cache-references          #   66.804 M/sec      (83.34%)
       283,141,591      cache-misses              #   41.556 % of all cache refs      (83.35%)
       845,986,965      branches                  #   82.945 M/sec      (83.34%)
       131,691,684      branch-misses             #   15.57% of all branches      (83.30%)
       
      14.001321572 seconds time elapsed

2020011000@w3:~/dsrw/other projects/b_tree_test$ pidof bt_main
13585
2020011000@w3:~/dsrw/other projects/b_tree_test$ perf stat -p 13585 -e task-clo
ck,cycles,instructions,cache-references,cache-misses,branches,branch-misses

 Performance counter stats for process id '13585':

          4,844.01 msec task-clock                #    0.692 CPUs utilized
    20,069,054,626      cycles                    #    4.143 GHz      (83.33%)
     3,062,262,303      instructions              #    0.15  insn per cycle      (83.33%)
     1,061,660,307      cache-references          #  219.170 M/sec      (83.34%)
       400,022,699      cache-misses              #   37.679 % of all cache refs      (83.33%)
       895,246,239      branches                  #  184.815 M/sec      (83.33%)
       110,397,542      branch-misses             #   12.33% of all branches      (83.34%)
       
       7.000631729 seconds time elapsed
```

