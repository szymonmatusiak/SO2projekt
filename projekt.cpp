/* g++ -std=c++11 -pthread projekt.cpp -o projekt.out -lncurses*/  
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string> 
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <unistd.h>
using namespace std;

queue < int > kolejka1;
mutex resourceKolejka1Guard;

queue < int > kolejka2;
mutex resourceKolejka2Guard;

string matrix[SIZE][SIZE];
mutex resourceMatrixGuard;

int zrodlo1, zrodlo2;
const int SIZE = 30;

void producent(){
	while(true){
      resourceKolejka1Guard.lock();
      resourceKolejka2Guard.lock();

		kolejka1.push(rand() % SIZE);
		kolejka2.push(rand() % SIZE);

      resourceKolejka1Guard.unlock();
      resourceKolejka2Guard.unlock();
	  sleep(1);
	}
}
void przetwarzacz(){
	while(true){
	  while(kolejka1.empty() == true);
	  resourceKolejka1Guard.lock();
      resourceKolejka2Guard.lock();
		
		zrodlo1 = kolejka1.front();
		zrodlo2 = kolejka2.front();	
		kolejka1.pop();
		kolejka2.pop();
      resourceKolejka1Guard.unlock();
      resourceKolejka2Guard.unlock();

	  resourceMatrixGuard.lock();
	
		matrix[zrodlo1][zrodlo2] = "*";
	
	  resourceMatrixGuard.unlock();
	}
}

void ncurses(){
	initscr();	
	while(true){
		erase();  
		for(int i=0; i < SIZE; i++){
			for(int j=0; j < SIZE; j++){
				if(matrix[i][j] == "*"){
					matrix[i][j] = " ";
					matrix[i][(j+1) % SIZE] = "*";
					j++;
				} 
			}
		}
		for(int i=0; i < SIZE; i++){
			for(int j=0; j < SIZE; j++){
				printw("%s", matrix[i][j].c_str());
			}
		printw("\n");
		}
	refresh();
	sleep(1);
	}

}

int main(){
    srand(time(NULL)); //seeding for the first time only!
	for(int i=0; i < SIZE; i++){
		for(int j=0; j < SIZE; j++){
			matrix[i][j] = " ";
		}	
	}
	//initscr();				/* start the curses mode */
 	auto producentThread = thread(producent); // startuje dany watek
  	auto przetwarzaczThread = thread(przetwarzacz); // startuje dany watek
	auto ncursesThread = thread(ncurses); // startuje dany watek
 	producentThread.join();	     
  	przetwarzaczThread.join();	   
	ncursesThread.join();	   
	
	refresh();
	getch();
	endwin();
 	
	return 0;
}

