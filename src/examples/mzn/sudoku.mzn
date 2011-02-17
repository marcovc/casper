% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_cd_fd
% RUNS ON zinc_fdic
%
%simple sudoku for squares of arbitrary size N = (S x S), for +ve integers S.
%
%andrew slater, july 2006
%

% MiniZinc version
% Peter Stuckey September 30 2006

int: S;
int: N = S * S;

set of int: PuzzleRange = 1..N;
set of int: SubSquareRange = 1..S;

array[1..N,1..N] of var PuzzleRange: puzzle;

% All different in rows and all different in columns.
%
constraint 
	forall (i, j in PuzzleRange, k in 1..(j - 1)) (
		puzzle[i,j] != puzzle[i,k]
	/\	puzzle[j,i] != puzzle[k,i]
	);

% All different in sub-squares:
% for the set of global coordinates (abscissa,ordinate) of the sub-squares 
% take a set of local coordinates of each sub-square,
% and for each coordinate, given a second set of local sub-square coordinates,
% if the coordinates differ, the value in the puzzle is different
%
constraint
	forall (
		a, o, a1, o1, a2, o2 in SubSquareRange
		where	a1 != a2
		/\	o1 != o2
	) (
		puzzle[(a - 1) * S + a1, (o - 1) * S + o1] !=
			puzzle[(a - 1) * S + a2, (o - 1) * S + o2]
	);

solve satisfy;

%
% The data for the puzzle that causes satz to make 1 backtrack (normally none 
% are made).
%

S=3;
puzzle=[|_,9,_,7,_,_,8,6,_|
          _,3,1,_,_,5,_,2,_|
          8,_,6,_,_,_,_,_,_|
          _,_,7,_,5,_,_,_,6|
          _,_,_,3,_,7,_,_,_|
          5,_,_,_,1,_,7,_,_|
          _,_,_,_,_,_,1,_,9|
          _,2,_,6,_,_,3,5,_|
          _,5,4,_,_,8,_,7,_|
|];

output [
	"sudoku:\n",
	show(puzzle[1, 1]), " ",
	show(puzzle[1, 2]), " ",
	show(puzzle[1, 3]), "  ",
	show(puzzle[1, 4]), " ",
	show(puzzle[1, 5]), " ",
	show(puzzle[1, 6]), "  ",
	show(puzzle[1, 7]), " ",
	show(puzzle[1, 8]), " ",
	show(puzzle[1, 9]), "\n",
	show(puzzle[2, 1]), " ",
	show(puzzle[2, 2]), " ",
	show(puzzle[2, 3]), "  ",
	show(puzzle[2, 4]), " ",
	show(puzzle[2, 5]), " ",
	show(puzzle[2, 6]), "  ",
	show(puzzle[2, 7]), " ",
	show(puzzle[2, 8]), " ",
	show(puzzle[2, 9]), "\n",
	show(puzzle[3, 1]), " ",
	show(puzzle[3, 2]), " ",
	show(puzzle[3, 3]), "  ",
	show(puzzle[3, 4]), " ",
	show(puzzle[3, 5]), " ",
	show(puzzle[3, 6]), "  ",
	show(puzzle[3, 7]), " ",
	show(puzzle[3, 8]), " ",
	show(puzzle[3, 9]), "\n\n",
	show(puzzle[4, 1]), " ",
	show(puzzle[4, 2]), " ",
	show(puzzle[4, 3]), "  ",
	show(puzzle[4, 4]), " ",
	show(puzzle[4, 5]), " ",
	show(puzzle[4, 6]), "  ",
	show(puzzle[4, 7]), " ",
	show(puzzle[4, 8]), " ",
	show(puzzle[4, 9]), "\n",
	show(puzzle[5, 1]), " ",
	show(puzzle[5, 2]), " ",
	show(puzzle[5, 3]), "  ",
	show(puzzle[5, 4]), " ",
	show(puzzle[5, 5]), " ",
	show(puzzle[5, 6]), "  ",
	show(puzzle[5, 7]), " ",
	show(puzzle[5, 8]), " ",
	show(puzzle[5, 9]), "\n",
	show(puzzle[6, 1]), " ",
	show(puzzle[6, 2]), " ",
	show(puzzle[6, 3]), "  ",
	show(puzzle[6, 4]), " ",
	show(puzzle[6, 5]), " ",
	show(puzzle[6, 6]), "  ",
	show(puzzle[6, 7]), " ",
	show(puzzle[6, 8]), " ",
	show(puzzle[6, 9]), "\n\n",
	show(puzzle[7, 1]), " ",
	show(puzzle[7, 2]), " ",
	show(puzzle[7, 3]), "  ",
	show(puzzle[7, 4]), " ",
	show(puzzle[7, 5]), " ",
	show(puzzle[7, 6]), "  ",
	show(puzzle[7, 7]), " ",
	show(puzzle[7, 8]), " ",
	show(puzzle[7, 9]), "\n",
	show(puzzle[8, 1]), " ",
	show(puzzle[8, 2]), " ",
	show(puzzle[8, 3]), "  ",
	show(puzzle[8, 4]), " ",
	show(puzzle[8, 5]), " ",
	show(puzzle[8, 6]), "  ",
	show(puzzle[8, 7]), " ",
	show(puzzle[8, 8]), " ",
	show(puzzle[8, 9]), "\n",
	show(puzzle[9, 1]), " ",
	show(puzzle[9, 2]), " ",
	show(puzzle[9, 3]), "  ",
	show(puzzle[9, 4]), " ",
	show(puzzle[9, 5]), " ",
	show(puzzle[9, 6]), "  ",
	show(puzzle[9, 7]), " ",
	show(puzzle[9, 8]), " ",
	show(puzzle[9, 9]), "\n"
];
