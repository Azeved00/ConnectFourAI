#ifndef _BASE_
#include "../base.cpp"
#endif

#ifndef _MATH_
#include <cmath>
#endif

/*
 * alpha beta algorithm
 *  this algorithm uses minmax as a base but tries to cut corners by
 *  not developing boards that will not affect the outcome
 *	efectively prunning that branch of the tree
 *
 *	pruning largely depends on wich values we check first so
 *	we will have a custom developing order to try to do as much prunning as possible
 *	order will be from certer to outer colluns and the left coluns before the right ones
 *
 * code will be almost identical to the min max algorithm,
 * difference is that we have extra parameters to evaluate when to prune or not to prune a branch
 *
 */
// this is the custom order
int order[] = {3,2,4,1,5,0,6};
int prunnedBoards;
int auxAB(Board* root, bool who,int alpha ,int beta,int depth)
{	
	// if the move is not possible
	// assing infinity so that we avoid the move
	if(root == NULL)
	{
		delete root;
		return (!who)*NINF + (who)*PINF;
	}
	seenBoards++;
	//check if there is a tie
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
		Board* child = root->makeMove(order[i],who);
		temp = auxAB(child, !who ,alpha, beta, depth-1);
		//check if there is change 
		//if there is update best value
		bool change = (who)*(temp > best) + (!who)*(temp < best);
		best = temp*change + best*(!change);
	
		//update alpha and beta
		//if alpha greater than beta break
		alpha = alpha*who*(alpha>=temp) + temp*who*(temp>alpha) + alpha*(!who);
		beta = beta*(!who)*(beta<=temp) + temp*(!who)*(temp < beta) + beta*who;
		if(beta<=alpha)
			break;
		
	}
	delete root;
	return best;
}

int alphaBeta(Board* root, int depth)
{
	prunnedBoards = 0;
	// check the board
	if(root == NULL) 
	{
		delete root;
		return -1;
	}
	seenBoards=0;
	
	int heuristic = root->heuristic();
	if(root->tie()||depth <= 0 || heuristic == 512 || heuristic == -512) 
	{
		delete root;
		return -1;
	}

	// play will save the best play
	// starts at 3 by default (order[0]) 
	int best = NINF, temp, play=3;	
	//alpha and beta are set here
	int alpha = NINF, beta = PINF;	
	for(int i=0; i < 7;i++)
	{
		Board* child = root->makeMove(order[i],1);
		temp = auxAB(child, 0, alpha, beta, depth-1);

		bool change = (temp > best);
		//here we also update the play if there is change
		play  = i*change + play*(!change);
		best = temp*change + best*(!change);

		//update alpha and beta
		//if alpha greater than beta break
		//here beta will never change but alpha can change
		alpha = alpha*(alpha>=temp) + temp*(temp>alpha);
		if(beta<=alpha) break;
	}

	prunnedBoards = pow(7,depth) - seenBoards;
	prunnedBoards *= 0.8;
	//we return the best play available 
	return order[play];
}




