#ifndef _ROWHEURISTIC_H_
#define _ROWHEURISTIC_H_

#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include "heap.h"
#include "board.h"

/*Heuristica sobre linha/coluna
	Calcula o numero de possibilidades de cada linha/coluna e preenche primeiro as mais restritivas
	Foi usada como fila de prioridade uma heap
*/

bool solveFullRow(Board &b, int x, int y, vector<struct HeapItem> &heap, int &count);

//resolve coluna inteira antes de passar ao proximo item da heap
//funcao chamada inicialmente pela funcao principal solveByRow
bool solveFullColumn(Board &b, int x, int y, vector<struct HeapItem> &heap, int &count){
	
	//coluna terminada
	if (x > b.n){
		//se heap esta vazia --> puzzle terminado
		if(heap.size()==0)
			return true;

		//atualiza heap(linhas)
		for(int k=0; k<(int)heap.size(); k++){
			if(heap[k].x==ROW){
				heap[k].key= solve_row(b, heap[k].y);
				if(heap[k].key==0)
					return false;
			}
		}
		make_heap(heap.begin(), heap.end(), Compare());
	
		//se primeiro item da linha tem 0 possibilidades
		if(heap[0].key==0){
			return false;
		}
	
		//pega primeiro item da heap
		int row, i, key;
		row=heap[0].x; //linha ou coluna
		i=heap[0].y; //indice da linha ou coluna
		key=heap[0].key; //chave (n. de possibilidades)
		pop_heap(heap.begin(), heap.end(), Compare());
		heap.pop_back();

		//Resolve coluna/linha retirada da heap
		bool success;
		if(row==ROW)
			success=solveFullRow(b, i, 1, heap, count);
		else
			success=solveFullColumn(b, 1, i, heap, count);

		//falhou ao achar solucao
		if(!success){
			//readiciona ultimo item a heap
			HeapItem h = HeapItem(row, i, key);
			heap.push_back(h);

			//atualiza heap(linhas)
			for(int k=0; k<(int)heap.size(); k++){
				if(heap[k].x==ROW){
					heap[k].key= solve_row(b, heap[k].y);
				}
			}
	
			make_heap(heap.begin(), heap.end(), Compare());
			return false;
		}
		
		return true;	
	}

	//Preenchimento do pixel atual

	//pixel ja preenchido
	if(b.mat[x][y]!=NONE){
		if (valid_row(b, x) and valid_col(b, y) and solveFullColumn(b, x+1, y, heap, count)){ //chama proximo item na coluna
			return true;
		}else
			return false;
	}

	b.mat[x][y] = BLACK;
	count++;
	if (valid_row(b, x) and valid_col(b, y) and solveFullColumn(b, x+1, y, heap, count)){
		return true;
	}

	b.mat[x][y] = WHITE;
	count++;

	if (valid_row(b, x) and valid_col(b, y) and solveFullColumn(b, x+1, y, heap, count)){
		return true;
	}

	b.mat[x][y] = NONE;
	count++;

	return false;
}

//Resolve uma linha inteira antes de pegar o proximo item da heap
//funcao chamada pela funcao principal solveByR
bool solveFullRow(Board &b, int x, int y, vector<struct HeapItem> &heap, int &count){
	
	//linha terminada
	if (y > b.m){
		//se heap esta vazia -> terminou o puzzle
		if(heap.size()==0)
			return true;

		//atualiza heap(colunas)
		for(int k=0; k<(int)heap.size(); k++){
			if(heap[k].x==COLUMN){
				heap[k].key= solve_col(b, heap[k].y);
				if(heap[k].key==0)
					return false;
			}
		}
		make_heap(heap.begin(), heap.end(), Compare());

		//se primeiro item da linha tem 0 possibilidades
		if(heap[0].key==0)
			return false;
		
		//pega primeiro item da heap
		int row, i, key;
		row=heap[0].x; //linha ou coluna
		i=heap[0].y; //indice da linha ou coluna
		key=heap[0].key; //chave (n. de possibilidades)
		pop_heap(heap.begin(), heap.end(), Compare());
		heap.pop_back();

		//resolve item retirado da heap
		bool success;
		if(row==ROW)
			success=solveFullRow(b, i, 1, heap, count);
		else
			success=solveFullColumn(b, 1, i, heap, count);

		//se nao conseguiu obter solucao
		if(!success){
			//readiciona ultimo item a heap
			HeapItem h = HeapItem(row, i, key);
			heap.push_back(h);

			//atualiza heap(colunas)
			for(int k=0; k<(int)heap.size(); k++){
				if(heap[k].x==COLUMN){
					heap[k].key= solve_col(b, heap[k].y);
				}
			}

			make_heap(heap.begin(), heap.end(), Compare());
			return false;
		}
		
		return true;	
	}

	//pixel ja preenchido
	if(b.mat[x][y]!=NONE){
		if (valid_row(b, x) and valid_col(b, y) and solveFullRow(b, x, y+1, heap, count)){
			return true;
		}else
			return false;
	}

	b.mat[x][y] = BLACK;
	count++;
	if (valid_row(b, x) and valid_col(b, y) and solveFullRow(b, x, y+1, heap, count)){
		return true;
	}

	b.mat[x][y] = WHITE;
	count++;

	if (valid_row(b, x) and valid_col(b, y) and solveFullRow(b, x, y+1, heap, count)){
		return true;
	}

	b.mat[x][y] = NONE;
	count++;

	return false;
}


//Funcao principal para a solucao por esta heuristica
bool solveByRow(Board &b, int &count){
	int i;

	//cria heap - chave eh o n. de possibilidades para cada linha/coluna
	vector<struct HeapItem> heap;
	heap.resize(b.m+b.n);

	//insere linhas na heap
	for(i=1; i<=b.n; i++){
		 heap[i-1] = HeapItem(ROW, i, solve_row(b, i));
	};

	//insere colunas na heap
	for(i=1; i<=b.m; i++){
		heap[b.n+i-1] = HeapItem(COLUMN, i, solve_col(b, i));
	};

	make_heap(heap.begin(), heap.end(), Compare());

	//remove primeiro item da heap
	int row; //indica se eh uma coluna ou uma linha
	row=heap[0].x;
	i=heap[0].y;
	pop_heap(heap.begin(), heap.end(), Compare());
	heap.pop_back();
	
	if(row==ROW) //primeiro item eh linha
		return solveFullRow(b, i, 1, heap, count);
	else //primeiro item eh coluna
		return solveFullColumn(b, 1, i, heap, count);


}

#endif