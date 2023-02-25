#ifndef _BASE_
#define _BASE_

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<utility>

#define ROWS 6
#define COLS 7

#define PINF 10000000
#define NINF -10000000
//character that will represent the players
char Player   = 'X';
char Computer = 'O';

long long totalSeenBoards = 0,seenBoards;

class Board
{
	private:
		bool values[ROWS][COLS];
		bool visited[ROWS][COLS];

		//Creating a new board from an already existing configuration
		Board(bool nValues[ROWS][COLS],bool nPlaces[ROWS][COLS]);
		
		public:

		//Creating a new empty Board
		Board();
		
		//simply make a move, 
		//returns a pointer to a board where the move was made
		Board* makeMove(int pos,bool player);

		//takes input to make a move
		Board* playerMove();
		//uses a designated algortihm to make a move
		Board* computerMove(int pos);

		//calculates the value of a certain board
		short heuristic();
		friend short slidingWindow(Board* board, int xI, int yI, int dX, int dY, int times, int* score);

		//checks if a board is a tie or not
		bool tie();

		//prints the board to standard input
		void printBoard();
};

//implementations
Board::Board()
{
	memset(values,0,sizeof values);
	memset(visited,0,sizeof visited);
}

Board::Board(bool nValues[ROWS][COLS], bool nVisited[ROWS][COLS])
{
	memcpy(values,nValues,sizeof values);
	memcpy(visited,nVisited,sizeof visited);
}

Board* Board::playerMove()
{
	int temp;
	printf("Enter column (1 to 7)\n");
	scanf("%d",&temp);

	Board* next = makeMove(temp-1,0);//computer is 1 and player is 0
		
	while(next == NULL)
	{
		printf("Move is invalid\n Try again:\n");
		scanf("%d",&temp);
		next = makeMove(temp-1,0);
	}

	return next;
}

Board* Board::computerMove(int pos)
{
	return makeMove(pos,1);
}	 

Board* Board::makeMove(int pos,bool player)
{
	if(pos < 0 || pos > 6 ) return NULL;
	
	//if the col is filled up
	if(this->visited[0][pos] != 0) return NULL;

	//find first empty space
	int i = ROWS-1;
	while(this->visited[i][pos] != 0)i--;

	//put letter in new configuration
	bool tempValues[ROWS][COLS];
	bool tempVisited[ROWS][COLS];
	
	memset(tempValues,0,sizeof tempValues);

	memcpy(tempValues, this->values, sizeof tempValues);
	memcpy(tempVisited,this->visited,sizeof tempVisited);
	
	tempValues[i][pos]=player;
	tempVisited[i][pos]=true;

	//create new board using updated configuration
	return new Board(tempValues,tempVisited);
}

void Board::printBoard()
{
	char temp;	
	printf("\033[2J");//this weird thing clears the board
	
	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLS; j++)
		{
			temp = 32 * (!this->visited[i][j]); //32 is decimal for space (see ascii table)
			temp += Computer * (this->visited[i][j] && this->values[i][j]);
			temp += Player * (this->visited[i][j] && !this->values[i][j]);
			printf("| %c ",temp);
		}	
		printf("|\n\n");
	}

	for(int i = 1; i < 8; i++)
		printf("  %d ",i);
	printf("\n");
}

short Board::heuristic()
{
	short sum =0;
	int score = 0;
	//diagonals left to right
	sum += slidingWindow(this,3,0,-1,1,1,&score);	
	sum += slidingWindow(this,4,0,-1,1,2,&score);	
	sum += slidingWindow(this,5,0,-1,1,3,&score);	
	sum += slidingWindow(this,5,1,-1,1,3,&score);	
	sum += slidingWindow(this,5,2,-1,1,2,&score);	
	sum += slidingWindow(this,5,3,-1,1,1,&score);

	//diagonals tight to left 
	sum += slidingWindow(this,3,6,-1,-1,1,&score);	
	sum += slidingWindow(this,4,6,-1,-1,2,&score);	
	sum += slidingWindow(this,5,6,-1,-1,3,&score);	
	sum += slidingWindow(this,5,5,-1,-1,3,&score);	
	sum += slidingWindow(this,5,4,-1,-1,2,&score);	
	sum += slidingWindow(this,5,3,-1,-1,1,&score);
	

	//horizontal
	for(int i = 0; i < ROWS;i++)
		sum += slidingWindow(this,i,0,0,1,4,&score);
	
	//vertical
	for(int i = 0; i < COLS;i++)
		sum += slidingWindow(this,0,i,1,0,3,&score);
		
	return sum*(score==0) + 512*(score>=1) - 512*(score<=-1);
}

short calc(short C,short P,int* score)
{
	short sum = 0;
	sum +=  1*(C == 1 && !P);
	sum += 10*(C == 2 && !P);
	sum += 50*(C == 3 && !P);	
		
	sum -=  1*(P == 1 && !C);
	sum -= 10*(P == 2 && !C); 
	sum -= 50*(P == 3 && !C);
	
	*score += (C==4 && !P);
	*score -= (P==4 && !C);
	
	return sum;
}

short slidingWindow(Board* board,int xI,int yI, int dX, int dY, int times,int* score)
{
	short sum = 0;
	short countP = 0;
	short countC = 0;
	
	//initial segment
	for(int i = 0; i < 4; i++)
	{
		countC += (board->visited[xI][yI] && board->values[xI][yI] );	
		countP += (board->visited[xI][yI] && !board->values[xI][yI]);
		xI += dX;
		yI += dY;
	}
	sum+=calc(countC,countP,score);

	//each time sum the new one subtract the last one
	for(int i = 1; i < times; i++)
	{

		countC += (board->visited[xI][yI] && board->values[xI][yI]);
		countC -= (board->visited[xI - 4*dX][yI - 4*dY] && board->values[xI - 4*dX][yI - 4*dY]);
		
		countP += (board->visited[xI][yI] && !board->values[xI][yI]);
		countP -= (board->visited[xI - 4*dX][yI - 4*dY] && !board->values[xI - 4*dX][yI - 4*dY]);
		
		sum+=calc(countC,countP,score);
		
		xI += dX;
		yI += dY;
	}

	return sum;
}

bool Board::tie()
{
	bool tie = 1;
	for(int i = 0;i < 7; i++)
		tie = tie && (visited[0][i]);
	return tie;
}
#endif
