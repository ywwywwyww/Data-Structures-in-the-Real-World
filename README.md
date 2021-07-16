# Data-Structures-in-the-Real-World
course 'Data Structures in the Real World' in freshman year, summer semester



### B Tree vs. std::set

$10^7$ 次查询

```shell
2020011000@w3:~/dsrw/other projects/b_tree_test$ pidof rbt_main
14844
2020011000@w3:~/dsrw/other projects/b_tree_test$ perf stat -p 14844 -e task-clock,cycles,instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-icache-loads,L1-icache-load-misses,branches,branch-misses

 Performance counter stats for process id '14844':

         10,069.68 msec task-clock                #    0.915 CPUs utilized
    41,658,655,712      cycles                    #    4.137 GHz      (49.99%)
     2,785,254,190      instructions              #    0.07  insn per cycle      (49.99%)
       679,708,309      cache-references          #   67.500 M/sec      (49.99%)
       285,332,547      cache-misses              #   41.979 % of all cache refs      (49.99%)
     1,564,298,610      L1-dcache-loads           #  155.347 M/sec      (50.00%)
       278,295,887      L1-dcache-load-misses     #   17.79% of all L1-dcache hits    (50.01%)
        21,004,188      L1-icache-loads           #    2.086 M/sec      (50.01%)
         1,137,718      L1-icache-load-misses     #    5.42% of all L1-icache hits    (50.01%)
       845,487,346      branches                  #   83.964 M/sec      (50.01%)
       131,509,984      branch-misses             #   15.55% of all branches      (50.00%)

      11.000841391 seconds time elapsed

2020011000@w3:~/dsrw/other projects/b_tree_test$ pidof bt_main
14850
2020011000@w3:~/dsrw/other projects/b_tree_test$ perf stat -p 14850 -e task-clock,cycles,instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-icache-loads,L1-icache-load-misses,branches,branch-misses

 Performance counter stats for process id '14850':

          4,721.74 msec task-clock                #    0.674 CPUs utilized
    19,521,646,691      cycles                    #    4.134 GHz      (50.01%)
     2,850,202,004      instructions              #    0.15  insn per cycle      (50.06%)
     1,038,002,536      cache-references          #  219.835 M/sec      (50.06%)
       398,697,584      cache-misses              #   38.410 % of all cache refs      (50.06%)
     1,312,079,827      L1-dcache-loads           #  277.881 M/sec      (50.06%)
       438,963,900      L1-dcache-load-misses     #   33.46% of all L1-dcache hits    (49.99%)
        27,522,187      L1-icache-loads           #    5.829 M/sec      (49.94%)
         1,264,367      L1-icache-load-misses     #    4.59% of all L1-icache hits    (49.94%)
       837,967,913      branches                  #  177.470 M/sec      (49.94%)
       110,206,403      branch-misses             #   13.15% of all branches      (49.94%)

       7.000584882 seconds time elapsed

2020011000@w3:~/dsrw/other projects/b_tree_test$
```

可以看出 `B Tree ` 的 `L1-dcache` 的命中率高出了不少。



在调整了块大小之后，当阶数 `m=111` 时 能跑出 `2.270s`，比红黑树快了 `4.435x`.

详细结果见目录下的 `results` 文件夹.

