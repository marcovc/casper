

predicate all_different(array[int] of var int: x);  % distinct(x)
predicate all_disjoint(array[int] of var set of int: S); % disjoint(S)
predicate cumulative(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b); % cumulative(s,d,r,b)
predicate disjoint(var set of int: s1, var set of int: s2); % disjoint(s1,s2)
predicate element(var int: i, array[int] of var bool: x, var bool: y); % equal(elem(x,i),y)
predicate element(var int: i, array[int] of var int: x, var int: y); % equal(elem(x,i),y)
predicate lex_lesseq(array[int] of var int: x, array[int] of var int: y); % lessEqual(x,y)
predicate maximum(var int: m, array[int] of var int: x); % equal(max(x),m)
predicate minimum(var int: m, array[int] of var int: x); % idem
predicate partition_set(array[int] of var set of int: S, set of int: universe); % partition(S,universe)
predicate table(array[int] of var bool: x, array[int, int] of bool: t); % inTable(x,t)
predicate table(array[int] of var int: x, array[int, int] of int: t); % inTable(x,t)

include "inverse.mzn"
