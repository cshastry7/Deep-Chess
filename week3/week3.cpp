#include "chess.hpp"
#include <bits/stdc++.h>
#include <chrono>
using namespace std;
using namespace chess;

//we have to return the first move. take a pointer to a Board var and set it to the fen of winning path when depth = highest
int alphabetapruning(Board board, int alpha, int beta, int depth, Move& bestmove,int maxdepth){
    Movelist moves;
    movegen::legalmoves(moves, board);

    int player = int(board.sideToMove());

    if (moves.empty()){
        if (player == 0){
            if (board.inCheck()){return -1;}
        }//0 IS WHITE AND 1 IS BLACK
        else if (player == 1){
            if (board.inCheck()){return 1;}
        }
        return 0;
    }

    else if (depth == 0) {return 0;}

    if (player == 0){
        int maxEval = -2;
        for (const auto &move : moves){
            board.makeMove(move);
            int eval = alphabetapruning(board,alpha,beta,depth-1,bestmove,maxdepth);
            if (eval>maxEval && depth==maxdepth){bestmove = move;}
            board.unmakeMove(move);
            maxEval = max(eval, maxEval);
            alpha = max(alpha, eval);
            if (beta<=alpha){break;}
        }
        return maxEval;
    }

    else{
        int minEval = 2;
        for (const auto &move : moves){
            board.makeMove(move);
            int eval = alphabetapruning(board,alpha,beta,depth-1,bestmove,maxdepth);
            if (eval<minEval && depth==maxdepth){bestmove = move;}
            board.unmakeMove(move);
            minEval = min(eval, minEval);
            beta = min(beta, eval);
            if (beta<=alpha){break;}
        }
        return minEval;
    }
}

int main(){
    string fen;
    int x=4; // CHANGE AS NEEDED FOR MATE IN x PUZZLES
    Move bestmove;
    getline(cin,fen);
    Board board(fen);
    int k = alphabetapruning(board,-2,2,x*2-1,bestmove,x*2-1);
    cout<<uci::moveToSan(board,bestmove)<<endl<<k<<endl;
}