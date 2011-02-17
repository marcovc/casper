/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

#include <casper/kernel.h>
#include <casper/int.h>

#include <casper/int/gacschema.h>

#include <fstream>

using namespace casper;
using namespace std;

/*
 *  +   2   +
 *    +   +
 *  1   +   3
 *    +   +
 *  +   4   +
 */

const UInt	size = 16;
const UInt	ncolors = 22;

const UInt	ntiles = 2 * (1+size) * size;

void piecePosToTilePos(UInt piecePos,
					   UInt& t1pos, UInt& t2pos,UInt& t3pos,UInt& t4pos)
{
	UInt pieceRow = piecePos / size;
	UInt pieceCol = piecePos % size;
	UInt pieceDiag = pieceRow + pieceCol;

	t1pos = 0;
	t4pos = 0;

	if (pieceDiag < size - 1)	// upper left triangle
	{
		t1pos = pieceDiag * (1+pieceDiag) + pieceCol * 2;
		t4pos = t1pos + 2*(pieceDiag+1) + 1;
	}
	else
	if (pieceDiag == size - 1)  // main diagonal
	{
		t1pos = pieceDiag * (1+pieceDiag) + pieceCol * 2;
		t4pos = t1pos + 2*(pieceDiag+1);
	}
	else						// lower right triangle
	{
		UInt invPieceDiag = size+size-pieceDiag-2;
		t4pos = ntiles - 2 -
				(invPieceDiag * (1+invPieceDiag) + (size-1-pieceCol) * 2);
		t1pos = t4pos - 2*(invPieceDiag+1) - 1;
	}
	t2pos = t1pos+1;
	t3pos = t4pos+1;
}


// labels one diagonal, or part of it
// (direction selects the direction for labeling: 0-NW,1-NE,2-SE,3-SW)
struct LabelDiagonal : IGoal
{
	LabelDiagonal(DomVarArray<Int> color,
				  UInt startRow, UInt startCol, UInt direction) :
		IGoal(color.solver()),color(color),
		startRow(startRow),startCol(startCol),
		direction(direction) {}
	Goal execute()
	{
		Int r = startRow;
		Int c = startCol;
		UInt tilepos=0;
		while (r < (Int)size and c < (Int)size and r >= 0 and c >= 0)
		{
			UInt t1pos,t2pos,t3pos,t4pos;
			// get diagonal tiles
			piecePosToTilePos(r*size+c,t1pos,t2pos,t3pos,t4pos);
			if (direction==0 and !color[t3pos].ground())
			{
				tilepos = t3pos;
				break;
			}
			else
			if (direction==1 and !color[t1pos].ground())
			{
				tilepos = t1pos;
				break;
			}
			else
			if ((direction==0 or direction==1) and !color[t2pos].ground())
			{
				tilepos = t2pos;
				break;
			}
			else
			if (direction==2 and !color[t1pos].ground())
			{
				tilepos = t1pos;
				break;
			}
			else
			if (direction==3 and !color[t3pos].ground())
			{
				tilepos = t3pos;
				break;
			}
			else
			if ((direction==2 or direction==3) and !color[t4pos].ground())
			{
				tilepos = t4pos;
				break;
			}
			if (direction == 0)	{	--r; --c; }
			else
			if (direction == 1)	{	--r; ++c; }
			else
			if (direction == 2)	{	++r; ++c; }
			else
			if (direction == 3)	{	++r; --c; }
		}
		if (r>=(Int)size or r<0 or c>=(Int)size or c<0)
			return ok();
		//std::cout << "enumerating tile " << tilepos << std::endl;
		return (color[tilepos] == color[tilepos].domain().min() or
			    color[tilepos] != color[tilepos].domain().min()) and
			    Goal(this);
	}
	DomVarArray<Int>	color;
	UInt				startRow;
	UInt				startCol;
	UInt				direction;
};

// labels one diagonal, or part of it
// (direction selects the direction for labeling: 0-NW,1-NE,2-SE,3-SW)
struct LabelDiagonalPieces : IGoal
{
	LabelDiagonalPieces(DomVarArray<Int,2> piece,
				  	UInt startRow, UInt startCol, UInt direction) :
		IGoal(piece.solver()),piece(piece),
		startRow(startRow),startCol(startCol),
		direction(direction) {}
	Goal execute()
	{
		Int r = startRow;
		Int c = startCol;
		while (r < (Int)size and c < (Int)size and r >= 0 and c >= 0)
		{
			if (!piece[r][c].ground())
				break;
			if (direction == 0)	{	--r; --c; }
			else
			if (direction == 1)	{	--r; ++c; }
			else
			if (direction == 2)	{	++r; ++c; }
			else
			if (direction == 3)	{	++r; --c; }
		}
		if (r>=(Int)size or r<0 or c>=(Int)size or c<0)
			return ok();
		//std::cout << "enumerating tile " << tilepos << std::endl;
		return (piece[r][c] == piece[r][c].domain().min() or
				piece[r][c] != piece[r][c].domain().min()) and
			    Goal(this);
	}
	DomVarArray<Int,2>	piece;
	UInt				startRow;
	UInt				startCol;
	UInt				direction;
};

Goal labelDiagonal(DomVarArray<Int> color,
				   UInt startRow,UInt startCol,UInt direction)
{	return new (color.solver().heap()) LabelDiagonal(color,startRow,startCol,direction);	}

Goal labelDiagonalPieces(DomVarArray<Int,2> piece,
				   UInt startRow,UInt startCol,UInt direction)
{	return new (piece.solver().heap()) LabelDiagonalPieces(piece,startRow,startCol,direction);	}

//Goal labelCross(DomVarArray<Int> color)
//{	return labelLeg(color,1) and labelLeg(color,0);	}

//Goal labelLegPieces(DomVarArray<Int,2> piece,Bool ullr = true)
//{	return new (piece.solver().heap()) LabelLegPieces(piece,ullr);	}

//Goal labelCrossPieces(DomVarArray<Int,2> piece)
//{	return labelLegPieces(piece,1) and labelLegPieces(piece,0);	}

void printC(UInt nconstraints, UInt c, Bool eq)
{
	cout << "<constraint name=\"C" << nconstraints
		<< "\" arity=\"1\" scope=\"VC" << c
		<< "\" reference=\"P" << !eq << "\">"
	    << "<parameters>VC" << c << " 0</parameters>"
	    << "</constraint>\n";
}

void win2Million(char* inFile)
{
	ICPSolver& solver;

	// which piece in each piece position
	DomVarArray<Int,2> piece(solver,size,size,0,size*size-1);

	for (UInt i = 0; i < size; i++)
		for (UInt j = 0; j < size; j++)
			cout << "<variable name=\"VP" << i*size+j
				 << "\" domain=\"DPiece\"/>\n";

	// which rotation in each piece position
	DomVarArray<Int,2> rotation(solver,size,size,0,3);

	for (UInt i = 0; i < size; i++)
		for (UInt j = 0; j < size; j++)
			cout << "<variable name=\"VR" << i*size+j
				 << "\" domain=\"DRotation\"/>\n";

	// which color in each tile position (0 is border)
	DomVarArray<Int> color(solver,ntiles,0,ncolors);

	for (UInt i = 0; i < ntiles; i++)
			cout << "<variable name=\"VC" << i
				 << "\" domain=\"DColor\"/>\n";

	PosTableRel<Int> table(solver,6,0,size*size-1,size*size*4);

	cout << "<relations nbRelations=\"1\">\n";
	cout << "<relation name=\"R0\" arity=\"6\" nbTuples=\""
		 << size*size*4 << "\" semantics=\"supports\">\n";

	UInt npiece = 0;
	ifstream f(inFile);
	while (!f.eof())
	{
		UInt color[4];
		for (UInt i = 0; i < 4; i++)
			f >> color[i] >> ws;
		for (UInt rot = 0; rot < 4; rot++)
		{
			table[npiece*4+rot][0] = npiece;
			table[npiece*4+rot][1] = rot;
			cout << npiece << " " << rot;
			for (UInt i = 0; i < 4; i++)
				table[npiece*4+rot][2+((i+rot)%4)] = color[i];
			for (UInt i = 0; i < 4; i++)
				cout << " " << table[npiece*4+rot][2+i];
			cout << " | ";
		}
		npiece++;
	}
	f.close();
//	cout << table << endl;
	cout << "</relation>\n";
	cout << "</relations>\n";

	Array<PosTableRel<Int>*>	tables(piece.count());
	for (UInt i = 0; i < piece.count(); i++)
		tables[i] = new PosTableRel<Int>(solver,table);

	cout << "<constraints nbConstraints=\""
		 << piece.count() << "\">\n";

	// post adjacency constraints
	for (UInt i = 0; i < piece.count(); i++)
	{
		UInt t1,t2,t3,t4;
		piecePosToTilePos(i,t1,t2,t3,t4);
		DomVarArray<Int> v(solver,6);
		v[0] = piece(i);
		v[1] = rotation(i);
		v[2] = color[t1];
		v[3] = color[t2];
		v[4] = color[t3];
		v[5] = color[t4];
		solver.post(gacSchema(v,*tables[i]));
		cout << "<constraint name=\"C" << i
				<< "\" arity=\"6\" scope=\"";
		cout << "VP" << i << " VR" << i << " VC" << t1
			 << " VC" << t2 << " VC" << t3 << " VC" << t4;
		cout << "\" reference=\"R0\"/>\n";
	}

	cout << "<constraint name=\"C"
	 	 << piece.count() << "\" arity=\""
	 	 << piece.count() << "\" scope=\"";
 	for (UInt i = 0; i < piece.count(); i++)
 		cout << "VP" << i << " ";
	 cout << "\" reference=\"global:allDifferent\"/>\n";

	// the same piece cannot be at more than one position
	DomVarArray<Int> auxv(solver,piece.count());
	for (UInt i = 0; i < piece.count(); i++)
		auxv[i] = piece(i);
	solver.post( distinct(auxv), GAC);

	UInt nconstraints = piece.count()+1;

	// fix border colors, and remove border color from inner tiles
	UInt aux = 0;
	for (UInt i = 0; i < size; i++)
	{
		printC(nconstraints++,aux,true);
		solver.post(color[aux++]==0);

		for (UInt j = 0; j < i*2; j++)
		{
			printC(nconstraints++,aux,false);
			solver.post(color[aux++]!=0);
		}
		printC(nconstraints++,aux,true);
		solver.post(color[aux++]==0);
	}
	for (Int i = size-1; i >= 0; i--)
	{
		printC(nconstraints++,aux,true);
		solver.post(color[aux++]==0);
		for (Int j = 0; j < i*2; j++)
		{
			printC(nconstraints++,aux,false);
			solver.post(color[aux++]!=0);
		}
		printC(nconstraints++,aux,true);
		solver.post(color[aux++]==0);
	}
	cout.flush();

	// fix one piece
	solver.post(piece[8][7] == 138);
	solver.post(rotation[8][7] == 3);

	cout << "first propagation: ";
	Bool found = solver.solve();
	cout << found << endl;

	cout << piece << endl;
	cout << rotation << endl;
	cout << color << endl;

	cout << "solving:\n";
	found = found and solver.solve( label(piece, selectVarFF(piece)) );
	//found &= solver.solve( labelLeg(color) );
	//found &= solver.solve( labelCross(color) and labelCrossPieces(piece));
	/*found &= solver.solve( labelDiagonal(color,8,7,0) and
						   labelDiagonal(color,8,7,1) and
						   labelDiagonal(color,8,7,2) and
						   labelDiagonal(color,8,7,3) and
						   labelDiagonalPieces(piece,8,7,0) and
   						   labelDiagonalPieces(piece,8,7,1) and
   						   labelDiagonalPieces(piece,8,7,2) and
   						   labelDiagonalPieces(piece,8,7,3)
						   );
	*/
	UInt nsols = 0;
	if (found)
	{
		cout << "cross " << ++nsols << " : " << endl;
		cout << piece << endl;
		//found = solver.next();
	}
	cout << solver.stats() << endl;
	cout << solver.totalTime() << endl;
}

int main(int argc,char** argv)
{
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " pieces-filename\n";
		return 1;
	}
	win2Million(argv[1]);
}
