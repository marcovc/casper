solve satisfy;
int: S;
S = 3;
set of int: SubSquareRange = 1 .. S;
int: N = S * S;
set of int: PuzzleRange = 1 .. N;
array[1 .. N, 1 .. N] of var PuzzleRange: puzzle;
output 
  [ "sudoku:\n", show(puzzle[1, 1]), " ", show(puzzle[1, 2]), " ", 
    show(puzzle[1, 3]), "  ", show(puzzle[1, 4]), " ", show(puzzle[1, 5]), 
    " ", show(puzzle[1, 6]), "  ", show(puzzle[1, 7]), " ", 
    show(puzzle[1, 8]), " ", show(puzzle[1, 9]), "\n", show(puzzle[2, 1]), 
    " ", show(puzzle[2, 2]), " ", show(puzzle[2, 3]), "  ", 
    show(puzzle[2, 4]), " ", show(puzzle[2, 5]), " ", show(puzzle[2, 6]), 
    "  ", show(puzzle[2, 7]), " ", show(puzzle[2, 8]), " ", 
    show(puzzle[2, 9]), "\n", show(puzzle[3, 1]), " ", show(puzzle[3, 2]), 
    " ", show(puzzle[3, 3]), "  ", show(puzzle[3, 4]), " ", 
    show(puzzle[3, 5]), " ", show(puzzle[3, 6]), "  ", show(puzzle[3, 7]), 
    " ", show(puzzle[3, 8]), " ", show(puzzle[3, 9]), "\n\n", 
    show(puzzle[4, 1]), " ", show(puzzle[4, 2]), " ", show(puzzle[4, 3]), 
    "  ", show(puzzle[4, 4]), " ", show(puzzle[4, 5]), " ", 
    show(puzzle[4, 6]), "  ", show(puzzle[4, 7]), " ", show(puzzle[4, 8]), 
    " ", show(puzzle[4, 9]), "\n", show(puzzle[5, 1]), " ", 
    show(puzzle[5, 2]), " ", show(puzzle[5, 3]), "  ", show(puzzle[5, 4]), 
    " ", show(puzzle[5, 5]), " ", show(puzzle[5, 6]), "  ", 
    show(puzzle[5, 7]), " ", show(puzzle[5, 8]), " ", show(puzzle[5, 9]), 
    "\n", show(puzzle[6, 1]), " ", show(puzzle[6, 2]), " ", 
    show(puzzle[6, 3]), "  ", show(puzzle[6, 4]), " ", show(puzzle[6, 5]), 
    " ", show(puzzle[6, 6]), "  ", show(puzzle[6, 7]), " ", 
    show(puzzle[6, 8]), " ", show(puzzle[6, 9]), "\n\n", show(puzzle[7, 1]), 
    " ", show(puzzle[7, 2]), " ", show(puzzle[7, 3]), "  ", 
    show(puzzle[7, 4]), " ", show(puzzle[7, 5]), " ", show(puzzle[7, 6]), 
    "  ", show(puzzle[7, 7]), " ", show(puzzle[7, 8]), " ", 
    show(puzzle[7, 9]), "\n", show(puzzle[8, 1]), " ", show(puzzle[8, 2]), 
    " ", show(puzzle[8, 3]), "  ", show(puzzle[8, 4]), " ", 
    show(puzzle[8, 5]), " ", show(puzzle[8, 6]), "  ", show(puzzle[8, 7]), 
    " ", show(puzzle[8, 8]), " ", show(puzzle[8, 9]), "\n", 
    show(puzzle[9, 1]), " ", show(puzzle[9, 2]), " ", show(puzzle[9, 3]), 
    "  ", show(puzzle[9, 4]), " ", show(puzzle[9, 5]), " ", 
    show(puzzle[9, 6]), "  ", show(puzzle[9, 7]), " ", show(puzzle[9, 8]), 
    " ", show(puzzle[9, 9]), "\n" ];
