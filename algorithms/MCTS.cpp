#ifndef _BASE_
#include "../base.cpp"
#endif

#ifndef _MATH_
#include <cmath>
#endif

#ifndef _RANDOM_
#include <cstdlib>
#include <ctime>
#endif

#define MAXT (1.0)
#define C (2)
/*
 * Monte Carlo Tree Search Algorithm (aka MCTS)
 *	this algorithm does not use a heuristic fucntion, 
 *	it simply does a huge ammount of random playouts
 *	and then, based on the result of those playouts, it will decide wich action to take
 *
 * it involves a couple of parts, mainly:*/
class Node; //to represent the tree
double playout(Node* root,bool who); // chose a leaf of the tree, based on Upper Confidence Bound 1 applied to trees(UCT)
//inside selection we also take the chosen leaf and develop 1 of its children(none of the children are developed)
double develop(Node* root,bool who);
/*
 * We will also have a new class to represent the nodes of the tree created by the algorithm
 *  and in that class there is the UCT involved in the selection function. 
 * Essencially UCT gives values to the children nodes and Selection will chose the highest value.
 */



class Node
{
	public:
		double balance;
		int playouts;
		Board* board;
		Node* children[7];

		double UCT(int parent) //parent is the playouts value of the parent node
		{
			if(!playouts) return PINF;
			double temp=0;
			temp += (double) balance / (double) playouts;
			temp += C * std::sqrt(std::log(parent) / playouts);
			return temp;
		}

		Node(Board* b)
		{
			playouts=0;
			balance = 0;
			board = b;
			for(int i = 0;i < 7;i++)
				children[i] = NULL;
		}
};

void del(Node* n)
{
	if(n == NULL)return;
	for (int i = 0; i < 7; i++)
	{
		if(n->children[i] == NULL) continue;
		delete n->children[i]->board;
		del(n->children[i]);
	}
	delete n;
	return;
}


int MCTS(Board* root)
{
	seenBoards =0;
	//playouts = 10;
	std::srand(std::time(0)); //seeding the random number generator to be used later
	Node* start = new Node(root);
	//stop after 0.5 seconds have passed
	clock_t time;
	time = clock();

	while((double)(clock() - time) / CLOCKS_PER_SEC < MAXT)
	{
		seenBoards++;
		playout(start,1); // selects a leaf node using UCT method and develop its children
	}
	//at the end we chose the child that has the highes playouts is chosen 
	int bestV= NINF, bestP = -1;
	for(int i = 0; i < 7; i++)
	{
		if(start->children[i]==NULL)continue;
		bool change = (start->children[i]->playouts) > bestV;
		bestV = bestV*(!change) + (start->children[i]->playouts)*change;
		bestP = bestP*(!change) + (i)*change;
	}
	del(start);
	return bestP;
}

double playout(Node *root, bool who)
{

	//check if root is a terminal state
	int h=root->board->heuristic();
	if(h==512 || h == -512 || root->board->tie())
	{
		double points = (h == -512)*(-1) + (h==512)*(1);
		root->playouts++;
		root->balance+=(points==-1)*(who); //add only to Nodes that are player
		root->balance+=(points==1)*(!who); //add only to Nodes that are computer
		return points;
	}



	double bestV = NINF;
	int bestP = -1;
	for(int i = 0; i<7; i++)
	{
		if(root->children[i] == NULL) continue;
		double temp = root->children[i]->UCT(root->playouts);	
		if(temp==PINF)
		{//if this children has 0 playouts
			bestP = i;
			break;
		}
		bool change = (temp > bestV);
		bestV = temp*change + bestV*(!change);
		bestP = i*change + bestP*(!change);
	}


	//backpropagation logic
	root->playouts++;	
	double points ;
	if(bestP != -1)
		points = playout(root->children[bestP],!who);
	else
		points = develop(root, who);

	root->balance+=(points==-1)*(who); //add only to Nodes that are player
	root->balance+=(points==1)*(!who); //add only to Nodes that are computer
	
	return points;
	
}

double develop(Node* root, bool who)
{
	//Create all of the children nodes
	for(int i = 0; i < 7; i++)
	{
		Board * temp = root->board->makeMove(i,who);
		if(temp == NULL) root->children[i] = NULL;
		else root->children[i] = new Node(temp);
	}

	bool child = who;
	//select (at random), one of the children 
	//to simulate a game
	int i = rand()%7;
	Board * simulation = root->board->makeMove(i,who);
	while(simulation == NULL) 
	{
		i = rand()%7; // randomize again
		simulation = root->board->makeMove(i,who);
	}
	int heuristic = simulation->heuristic();

	who = !who;
	Board* temp = simulation; 
	//simulate the game until a terminal state is reached
	while(!simulation->tie() && heuristic != 512 && heuristic != -512)
	{
		simulation = temp->makeMove(rand()%7,who);
		while(simulation == NULL)
			simulation = temp->makeMove(rand()%7,who);
		//deleting the new boards
		//so that they dont stay in memmory indefinetly
		delete temp;
		temp = simulation;
		heuristic = temp->heuristic();
		who = !who;	
	}	
	//at the end check the result
	//backpropagate it
	//starting the child in which the simulation occurred
	double points = (heuristic == -512)*(-1) + (heuristic==512)*(1);
	root->children[i]->playouts++;
	root->children[i]->balance+=(points==-1)*(child); //add only to Nodes that are player
	root->children[i]->balance+=(points==1)*(!child); //add only to Nodes that are computer
	return points;
}

