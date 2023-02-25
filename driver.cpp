
#ifndef _BASE_
#include "base.cpp"
#endif

#ifndef _TIME_
#define _TIME_
#include <time.h>
#endif

#include "./algorithms/minMax.cpp"
#include "./algorithms/alphaBeta.cpp"
#include "./algorithms/MCTS.cpp"

int main(int argC, char* argV[])
{
	int op,algo,depth=10,move=0;

	printf("\033[2J");
	printf("Do you want to play connect four? :)\n");
	printf("1 - yes, 0 - no\n");
	scanf("%d",&op);
	while(op)
	{
		printf("\nDo you want to be first?\n");
		printf("1 - yes, 0 - no\n");
		scanf("%d",&op);

		printf("\nWho do you want your oponent to be?\n");
		printf("Algorithms:\n1 - Min max,\n2 - Alpha Beta,\n3 - Monte Carlo Tree Search\n");
		scanf("%d",&algo);
		while(algo <= 0 || algo > 3)
		{
			printf("Please choose one algoritm from the list\n");
			scanf("%d",&algo);
		}

		if(algo == 1 || algo == 2)
		{
			printf("\nWhat do you want the max depth to be?\n");
			printf("If you chose minmax i recommend 3,4 or 5\n");
			printf("If you chose alpha beta i recommend 6,8 or even 10 for a challenge\n");
			printf("Remember if you chose a high value it may take a little to play\n\tbut it will be much harder to play aginst\n");
			printf("Note: if the value you chose is higher than the recommended\n\tthe AI may never play\n");
			scanf("%d",&depth);
		}
	
	
		//creating a new empty board
		Board* atual = new Board();
	
		// best first move is allways in the center collun
		// it is not up to chance
		if(op==0) atual = atual->computerMove(3);
	
		op=atual->heuristic();
		atual->printBoard();	
		
		while(1)
		{	
			//player logic
			atual = atual->playerMove(); // player moves
			op = atual->heuristic();	//check if the game ended
			if(op == -512)
			{
				printf("\033[2J");
				atual->printBoard();
				printf("\nCongratulations you Won!\n");
				break;
			}
			if(atual->tie())
			{
				printf("\033[2J");
				atual->printBoard();
				printf("\nYou tied, there are no winners today\n");
				break;
			}
			
			//clock
			clock_t time;
			time = clock();
	
			double timeTaken;
	
			//ai logic
			if(algo==1)
				move = minMax(atual,depth);
			else if(algo==2)
				move =alphaBeta(atual,depth);
			else
				move = MCTS(atual);
	
			time = clock();
			timeTaken = ((double) (time)) / CLOCKS_PER_SEC;
			
	
			atual = atual->computerMove(move);	//computer moves
			op = atual->heuristic();	//check if the game ended

			if(op==512)
			{
			
				printf("\033[2J");
				atual->printBoard();
				printf("\nBoo hoo, you lost to my algorithm :)\n");
				break;
			}
			if(atual->tie())
			{
				printf("\033[2J");
				atual->printBoard();
				printf("\nYou tied, there are no winners today\n");
				break;
			}
		
			//print board and stats
			//go back to beguining
		atual->printBoard();
		printf("\n------------Stats------------\n");
			printf("\nComputer placed chip on %d collun\n",move+1);
			printf("Value( according to player): %hd\n",-op);
	
			totalSeenBoards+=seenBoards;
			if(algo ==1 || algo == 2)
			{
				printf("Seen boards: %lld (%lld)\n",seenBoards,totalSeenBoards);
				if(algo==2)
					printf("Prunned boards(aproximation): %d\n",prunnedBoards);
				printf("Seconds taken: %lf\n",timeTaken);
			}
			
			//	there is a bug in MCTS that i cant count the time
			//	it just takes the sum of the time taken
			//	but the time is allways about 0.5 +- nanosecond
			else 
			{	
				printf("Playouts: %lld (%lld)\n",seenBoards,totalSeenBoards);
				printf("Seconds taken (aproximation): %lf\n",MAXT);
			}
		}
		printf("\nDo you want to play again?\n");
		printf("1 - yes, 0 - no\n");
		scanf("%d",&op);
		printf("\033[2J");
	}
}

