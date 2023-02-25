#ifndef _BASE_
#include "../base.cpp"
#endif


/*
 * min max algorithm
 * will see every possible move until up to a certain depth
 * by maximizing the value of its move and minimizing the value of the players move
 * it can determine what is the best possible move
 * 
 * envolves 2 funcitons:
 *	first is the driver fucntion 
 *		inputs: the current board and a depth
 *		returns: the best collun to put the chip in
 *
 *	the second function is the recursive and main function
 *		inputs: a board to analyze and develop, the player to simulate and the depth it is in
 *		returns: the value of the best move(according to the player)
 *
 * notes: in minMax function, it is only commented the things that are different from auxMM
 *	the stuff that is the same will only be commented in auxMM
 */
int auxMM(Board* root, bool who, int depth)
{
	
	// if the move is not possible
	// assing infinity so that we avoid the move
	if(root == NULL) 
	{
		delete root;
		return (!who)*NINF + (who)*PINF;
	}
		seenBoards++;
	//check if tie
	if(root->tie())
	{
		delete root;
		return 0;
	}

	//check if the current board is leaf or terminal state
	int heuristic = root->heuristic();
	if(depth <= 0 || heuristic == 512 || heuristic == -512) 
	{
		delete root;
		return heuristic;
	}	
	
	int best = who*NINF + (!who)*PINF , temp;	
	for(int i=0; i < 7;i++)
	{
		//try this move
		Board* child = root->makeMove(i,who);
		temp = auxMM(child,!who,depth-1);
		//check if there is change 
		//if there is update best value
		bool change = (who)*(temp > best) + (!who)*(temp < best);
		best = temp*change + best*(!change);
	}

	delete root;
	return best;
}

int minMax(Board* root, int depth)
{
	seenBoards = 0;
	// check the board
	if(root == NULL) 
	{
		delete root;
		return -1;
	}
	if(root->tie()) 
	{
		delete root;
		return -1;
	}
	
	int heuristic = root->heuristic();
	if(depth <= 0 || heuristic == 512 || heuristic == -512) 
	{
		delete root;
		return -1;
	}

	// play will save the best play
	// starts at 0 by default 
	int best = NINF, temp, play=0;	
	for(int i=0; i < 7;i++)
	{
		Board* child = root->makeMove(i,1);
		temp = auxMM(child,0,depth-1);

		bool change = (temp > best);
		//here we also update the play if there is change
		play  = i*change + play*(!change);
		best = temp*change + best*(!change);
	}
	
	//we return the best play available 
	return play;
}




