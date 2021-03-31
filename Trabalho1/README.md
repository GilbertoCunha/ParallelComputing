bubble: The code directly imported from portuguese wikipedia.
bubble1: 
    - Reduce number of inner cycles (subtract j)
    - Remove size checking (it is done before calling the function)
bubble2:
    - Change if statement order of comparison in inner cycle for memory hierarchy

| Métricas | Array Size |     CC      |     #I      |    CPI     |     L1 Misses     | Texe (us)  |
|----------|------------|-------------|-------------|------------|-------------------|------------|
|    v0    |   100000   | 66721030144 | 69752487936 |   0.957    | 2829873788 (1.6%) |  16137955  |
|    v1    |   100000   | 56398184448 | 61202292736 |   0.923    | 1561276188 (1.3%) |  13621298  |