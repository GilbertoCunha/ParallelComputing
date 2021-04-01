v0: The code directly imported from portuguese wikipedia.
v1.0: Optimize item distribution for each bucket
v1.1: 
    - Reduce number of inner cycles (subtract j)
    - Remove size checking (it is done before calling the function)
v1.2:
    - Change if statement order of comparison in inner cycle for memory hierarchy

| Métricas | Array Size  |     CC      |     #I      |    CPI     |      L1 Misses     | Texe (us)  |
|----------|-------------|-------------|-------------|------------|--------------------|------------|
|    v0    |   1000000   | 62043467776 | 77250641920 |   0.803    | 3130779722 (1.63%) |  14912454  |
|   v1.0   |   1000000   | 62234927104 | 77245448192 |   0.806    | 3131874562 (1.74%) |  14906693  |
|   v1.1   |   1000000   | 58708176896 | 62472904704 |   0.940    | 1563782751 (1.25%) |  14160646  |
|   v1.2   |   1000000   | 58580742144 | 62472904704 |   0.938    | 1563581405 (1.24%) |  14128795  |