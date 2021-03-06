#pragma once

#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

#define NONE 0
#define WHITE 1
#define BLACK 2

class Board{
public:
	vector<vector<int> > row; // (Constant) Restrições das linhas.
	vector<vector<int> > col; // (Constant) Restrições das colunas.
	vector<vector<int> > mat; // Matriz N x M (1-based).
	vector<vector<long long> > row_black; // Número de combinações da linha i nas quais a celula (i, j) é pintada de preto.
	vector<vector<long long> > row_white; // Número de combinações da linha i nas quais a celula (i, j) é pintada de branco.
	vector<vector<long long> > col_black; // Número de combinações da coluna j nas quais a celula (i, j) é pintada de preto.
	vector<vector<long long> > col_white; // Número de combinações da coluna j nas quais a celula (i, j) é pintada de branco.
	vector<long long> row_combinations; // Número de combinações da linha i.
	vector<long long> col_combinations; // Número de combinações da coluna j.
	int n, m; // (Constant) Dimensões do tabuleiro.

	// O(1) Construtor vazio.
	Board(){
		this->n = this->m = 0;
	}

	// O(N * M) Construtor.
	Board(int n, int m){
		int i;

		// Dimensões.
		this->n = n;
		this->m = m;

		// Restrições.
		this->row.resize(n + 1);
		this->col.resize(m + 1);

		// Número de combinações de cada linha/coluna.
		this->row_combinations.assign(n + 1, 0);
		this->col_combinations.assign(m + 1, 0);

		// Matriz principal.
		this->mat.resize(n + 1);

		// Número de combinações.
		this->row_black.resize(n + 1);
		this->row_white.resize(n + 1);
		this->col_black.resize(n + 1);
		this->col_white.resize(n + 1);

		// Alocando as colunas.
		for (i = 0; i <= n; i++){
			// Tabuleiro vazio.
			this->mat[i].assign(m + 1, NONE);

			// Número de combinações vazio.
			this->row_black[i].assign(m + 1, 0);
			this->row_white[i].assign(m + 1, 0);
			this->col_black[i].assign(m + 1, 0);
			this->col_white[i].assign(m + 1, 0);
		}
	}

	/* O(1) Retorna o símbolo de impressão de cada cor. */
	static char symbol(int type){
		if (type == WHITE){
			return '_';
		}

		if (type == BLACK){
			return '#';
		}

		return '.';
	}

	/* O(1) Retorna a cor oposta. */
	static int toggle(int color){
		return color == WHITE ? BLACK : WHITE;
	}

	/* O(N * M) Função que imprime o tabuleiro. */
	void print(){
		int krow_max, kcol_max, i, j;

		// Obtendo o número máximo de restrições-linha.
		for (i = 1, krow_max = 0; i <= n; i++){
			krow_max = max(krow_max, (int)row[i].size());
		}

		// Obtendo o número máximo de restrições-coluna.
		for (i = 1, kcol_max = 0; i <= m; i++){
			kcol_max = max(kcol_max, (int)col[i].size());
		}

		// Imprimindo dimensões do tabuleiro.
		printf("%d %d\n", n, m);

		// Imprimindo as restrições-coluna.
		for (i = 0; i < kcol_max; i++){
			for (j = 0; j < krow_max; j++){
				printf("   ");
			}

			for (j = 1; j <= m; j++){
				if (kcol_max - i <= (int)col[j].size()){
					printf("%3d", col[j][(int)col[j].size() - (kcol_max - i)]);
				}
				else{
					printf("   ");
				}
			}

			printf("\n");
		}

		for (i = 1; i <= n; i++){
			// Imprimindo as restrições-linha.
			for (j = 0; j < krow_max; j++){
				if (krow_max - j <= (int)row[i].size()){
					printf("%3d", row[i][(int)row[i].size() - (krow_max - j)]);
				}
				else{
					printf("   ");
				}
			}

			// Imprimindo o tabuleiro.
			for (j = 1; j <= m; j++){
				printf("%3c", symbol(mat[i][j]));
			}

			printf("\n");
		}
	}

	/* O(N * M) Imprime as possibilidades por linha e coluna. */
	void print_possibilities(){
		int i, j, k;

		// Para cada cor
		for (k = WHITE; k <= BLACK; k++){
			printf("\n%s possibilities:\n", k == WHITE ? "White" : "Black");
			
			// Imprimindo as combinações por linha.
			printf("\tRow possibilities:\n");

			for (i = 1; i <= n; i++){
				printf("%5lld: ", row_combinations[i]);

				for (j = 1; j <= m; j++){
					printf("%6lld ", k == WHITE ? row_white[i][j] : row_black[i][j]);
				}

				printf("\n");
			}

			// Imprimindo as combinações por coluna.
			printf("\n\tColumn possibilities:\n");
			printf("     ");

			for (j = 1; j <= m; j++){
				printf("%5lld: ", col_combinations[j]);
			}

			printf("\n");

			for (i = 1; i <= n; i++){
				printf("     ");

				for (j = 1; j <= m; j++){
					printf("%6lld ", k == WHITE ? col_white[i][j] : col_black[i][j]);
				}

				printf("\n");
			}
		}
	}

	/* O(1) Retorna o respectivo caractere para impressão em PGM. */
	char pgm_char(int color){
		return color == BLACK ? '1' : '0';
	}

	/* O(N * M) Imprime o tabuleiro em formato PGM. */
	void print_pgm(){
		int i, j;

		printf("P1\n");
		printf("%d %d\n", m, n);

		for (i = 1; i <= n; i++){
			for (j = 1; j < m; j++){
				printf("%c ", pgm_char(mat[i][j]));
			}

			printf("%c\n", pgm_char(mat[i][j]));
		}
	}

	/* O(Dimensão * Restrições * Restrição_Máxima). Aloca uma dp com dimensões AxBxC. */
	void alloc_dp(vector<vector<vector<long long> > > &dp, int a, int b, int c){
		int i, j;

		dp.resize(a);

		for (i = 0; i < a; i++){
			dp[i].resize(b);

			for (j = 0; j < b; j++){
				dp[i][j].resize(c);
			}
		}
	}

	/* O(Dimensão * Restrições * Restrição_Máxima). Inicializa a dp com -1. */
	void init_dp(vector<vector<vector<long long> > > &dp){
		int i, j;

		for (i = 0; i < (int)dp.size(); i++){
			for (j = 0; j < (int)dp[i].size(); j++){
				fill(dp[i][j].begin(), dp[i][j].end(), -1);
			}
		}
	}

	/* O(M * Restrições * Restrição_Máxima). Calcula o número de possibilidades para a linha x. */
	long long solve_row_aux(vector<vector<vector<long long> > > &dp, int x, int y, int p, int c){
		// Caso base.
		if (y == m + 1){
			return p == (int)row[x].size() and c <= 1;
		}

		// Estado já calculado.
		if (dp[y][p][c] != -1){
			return dp[y][p][c];
		}

		// Corte por impossibilidade.
		if ((c > 1 and mat[x][y] == WHITE) or (c <= 1 and mat[x][y] == BLACK)){
			return dp[y][p][c] = 0;
		}

		// Preto ou branco.
		if (p < (int)row[x].size() and c <= 1){
			return dp[y][p][c] = solve_row_aux(dp, x, y + 1, p + 1, row[x][p] + 1) + solve_row_aux(dp, x, y + 1, p, 0);
		}

		// Passando pra frente.
		return dp[y][p][c] = solve_row_aux(dp, x, y + 1, p, max(0, c - 1));
	}

	/* Função que calcula as possibilidades da linha x. */
	void solve_row(int x){
		vector<vector<vector<long long> > > dp;
		int k, y;

		// Inicializando a DP.
		k = *max_element(row[x].begin(), row[x].end());		
		alloc_dp(dp, m + 1, (int)row[x].size() + 1, k + 2);

		// Calculando o número de combinações da linha.
		init_dp(dp);
		row_combinations[x] = solve_row_aux(dp, x, 1, 0, 0) + solve_row_aux(dp, x, 1, 1, row[x][0] + 1);

		for (y = 1; y <= m; y++){
			if (mat[x][y] == BLACK){
				// Se já estiver pintado de preto.
				row_black[x][y] = row_combinations[x];
				row_white[x][y] = 0;
			}
			else if (mat[x][y] == WHITE){
				// Se já estiver pintado de branco.
				row_black[x][y] = 0;
				row_white[x][y] = row_combinations[x];
			}
			else{
				// Se ainda não estiver pintado devo calcular as possibilidades.
				init_dp(dp);
				mat[x][y] = BLACK;
				row_black[x][y] = solve_row_aux(dp, x, 1, 0, 0) + solve_row_aux(dp, x, 1, 1, row[x][0] + 1);
				
				init_dp(dp);
				mat[x][y] = WHITE;
				row_white[x][y] = solve_row_aux(dp, x, 1, 0, 0) + solve_row_aux(dp, x, 1, 1, row[x][0] + 1);

				mat[x][y] = NONE;
			}
		}
	}

	/* Função que calcula as possibilidades para a linha x entre [y..m] com as restrições entre [p..k-1]. */
	long long solve_col_aux(vector<vector<vector<long long> > > &dp, int y, int x, int p, int c){
		// Caso base.
		if (x == n + 1){
			return p == (int)col[y].size() and c <= 1;
		}

		if (dp[x][p][c] != -1){
			return dp[x][p][c];
		}

		// Corte.
		if ((c > 1 and mat[x][y] == WHITE) or (c <= 1 and mat[x][y] == BLACK)){
			return dp[x][p][c] = 0;
		}

		// Preto ou branco.
		if (p < (int)col[y].size() and c <= 1){
			return dp[x][p][c] = solve_col_aux(dp, y, x + 1, p + 1, col[y][p] + 1) + solve_col_aux(dp, y, x + 1, p, 0);
		}

		// Passando pra frente
		return dp[x][p][c] = solve_col_aux(dp, y, x + 1, p, max(0, c - 1));
	}

	/* Função que calcula as possibilidades da linha x. */
	void solve_col(int y){
		vector<vector<vector<long long> > > dp;
		int k, x;

		// Inicializando a DP.
		k = *max_element(col[y].begin(), col[y].end());
		alloc_dp(dp, n + 1, (int)col[y].size() + 1, k + 2);

		// Calculando o número de combinações da linha.
		init_dp(dp);
		col_combinations[y] = solve_col_aux(dp, y, 1, 0, 0) + solve_col_aux(dp, y, 1, 1, col[y][0] + 1);

		for (x = 1; x <= n; x++){
			if (mat[x][y] == BLACK){
				// Se já estiver pintado de preto.
				col_black[x][y] = col_combinations[y];
				col_white[x][y] = 0;
			}
			else if (mat[x][y] == WHITE){
				// Se já estiver pintado de branco.
				col_black[x][y] = 0;
				col_white[x][y] = col_combinations[y];
			}
			else{
				// Se ainda não estiver pintado devo calcular as possibilidades.
				init_dp(dp);
				mat[x][y] = BLACK;
				col_black[x][y] = solve_col_aux(dp, y, 1, 0, 0) + solve_col_aux(dp, y, 1, 1, col[y][0] + 1);
				
				init_dp(dp);
				mat[x][y] = WHITE;
				col_white[x][y] = solve_col_aux(dp, y, 1, 0, 0) + solve_col_aux(dp, y, 1, 1, col[y][0] + 1);

				mat[x][y] = NONE;
			}
		}
	}

	/* Função que calcula todas as possibilidades do tabuleiro inteiro. */
	void solve(){
		int x, y;

		// Calculando as possibilidades de todas as linhas.
		for (x = 1; x <= n; x++){
			solve_row(x);
			printf("Done line %d\n", x);
		}

		// Calculando as possibilidades de todas as colunas.
		for (y = 1; y <= m; y++){
			solve_col(y);
			printf("Done column %d\n", y);
		}
	}

	/* Função que verifica se a linha x está válida (possui alguma combinação possível). */
	bool valid_row(int x){
		solve_row(x);
		return row_combinations[x] > 0;
	}

	/* Função que verifica se a coluna y está válida (possui alguma combinação possível). */
	bool valid_col(int y){
		solve_col(y);
		return col_combinations[y] > 0;
	}

	/* Função que verifica se a linha x e a coluna y estão válidas. */
	bool valid(int x, int y){
		return valid_row(x) and valid_col(y);
	}
};