# Data-Structures-in-the-Real-World
course 'Data Structures in the Real World' in freshman year, summer semester

## PA1

### Part2

SIMD 就不加了好了？



测一下加 size 的时候的 cache miss 和加了 size 之后的吧.

修改之前的

```shell
2020011000@w3:~$ perf stat -e task-clock,cycles,instructions,cache-references,c
ache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-icache-loads,L1-icache-loa
d-misses,branches,branch-misses -p 124494

 Performance counter stats for process id '124494':

          3,316.28 msec task-clock                #    0.553 CPUs utilized

    13,725,665,009      cycles                    #    4.139 GHz
     8,738,541,318      instructions              #    0.64  insn per cycle
       330,174,644      cache-references          #   99.562 M/sec
       111,379,075      cache-misses              #   33.733 % of all cache refs
     4,446,684,514      L1-dcache-loads           # 1340.866 M/sec
       154,839,024      L1-dcache-load-misses     #    3.48% of all L1-dcache hits
     3,044,254,234      L1-icache-loads           #  917.973 M/sec
         5,444,384      L1-icache-load-misses     #    0.18% of all L1-icache hits
     1,739,065,412      branches                  #  524.403 M/sec
       146,303,418      branch-misses             #    8.41% of all branches

       6.000631735 seconds time elapsed

```



修改之后的

```shell
2020011000@w3:~$ perf stat -e task-clock,cycles,instructions,cache-references,c
ache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-icache-loads,L1-icache-loa
d-misses,branches,branch-misses -p 124805

 Performance counter stats for process id '124805':

          3,500.11 msec task-clock                #    0.583 CPUs utilized

    14,377,538,825      cycles                    #    4.108 GHz
     9,096,944,037      instructions              #    0.63  insn per cycle
       460,572,684      cache-references          #  131.588 M/sec
       145,881,047      cache-misses              #   31.674 % of all cache refs
     4,578,527,075      L1-dcache-loads           # 1308.109 M/sec
       216,118,771      L1-dcache-load-misses     #    4.72% of all L1-dcache hits
     3,928,179,736      L1-icache-loads           # 1122.301 M/sec
         6,586,966      L1-icache-load-misses     #    0.17% of all L1-icache hits
     1,816,624,066      branches                  #  519.019 M/sec
       146,100,117      branch-misses             #    8.04% of all branches

       6.000682789 seconds time elapsed

```



L1-dcache-load-misses 多了一些...

看来看去好像没啥大问题？可能只是爆缓存了？



接着又把 node size 调小了一点...

```shell
2020011000@w3:~$ perf stat -e task-clock,cycles,instructions,cache-references,c
ache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-icache-loads,L1-icache-loa
d-misses,branches,branch-misses -p 125988

 Performance counter stats for process id '125988':

          3,024.13 msec task-clock                #    0.756 CPUs 
    12,439,433,271      cycles                    #    4.113 GHz
     8,734,128,415      instructions              #    0.70  insn per cycle
       537,857,981      cache-references          #  177.855 M/sec
       164,372,756      cache-misses              #   30.561 % of all cache refs
     4,869,156,535      L1-dcache-loads           # 1610.102 M/sec
       248,913,975      L1-dcache-load-misses     #    5.11% of all L1-dcache hits
     2,631,303,106      L1-icache-loads           #  870.103 M/sec
         7,280,995      L1-icache-load-misses     #    0.28% of all L1-icache hits
     1,842,414,374      branches                  #  609.238 M/sec
       114,364,074      branch-misses             #    6.21% of all branches

       4.000395703 seconds time elapsed

```

反正是变快了。。。



剩下的等到发数据了再微调吧...



## Other projects

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



在调整了块大小之后，当阶数 `m=111` 时 能跑出 `2.270s`，比红黑树快了 `4.435x`.

```shell

 Performance counter stats for process id '15813':

          2,270.28 msec task-clock                #    0.757 CPUs utilized          
     9,469,385,284      cycles                    #    4.171 GHz                      (49.83%)
     9,102,713,936      instructions              #    0.96  insn per cycle           (49.83%)
       444,582,408      cache-references          #  195.827 M/sec                    (49.83%)
       142,199,531      cache-misses              #   31.985 % of all cache refs      (49.86%)
     1,536,226,932      L1-dcache-loads           #  676.669 M/sec                    (50.04%)
       218,731,554      L1-dcache-load-misses     #   14.24% of all L1-dcache hits    (50.17%)
        18,004,353      L1-icache-loads           #    7.930 M/sec                    (50.17%)
           476,000      L1-icache-load-misses     #    2.64% of all L1-icache hits    (50.17%)
     2,600,702,488      branches                  # 1145.544 M/sec                    (50.14%)
        50,667,378      branch-misses             #    1.95% of all branches          (49.96%)

       3.000377804 seconds time elapsed
```

详细结果见目录下的 `results` 文件夹.



### 并行计算-统计质数个数

统计 $[1, {10}^6]$ 内质数个数，切成 `#threads` 段分别统计.

| 线程数 | 运行时间(us) | 效率   |
| ------ | ------------ | ------ |
| 1      | 8748868      | 100%   |
| 2      | 5560359      | 78.67% |
| 3      | 3912365      | 74.54% |
| 4      | 3049737      | 71.72% |
| 5      | 2498937      | 70.02% |
| 6      | 2147586      | 67.90% |
| 7      | 1945037      | 64.26% |
| 8      | 1763587      | 62.01% |
| 9      | 1561146      | 62.27% |
| 10     | 1661819      | 52.65% |

