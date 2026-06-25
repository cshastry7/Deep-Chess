#include "chess.hpp"
#include <bits/stdc++.h>

using namespace std;
using namespace chess;

void parsePosition(Board& board, const std::string& input) {
    std::stringstream ss(input);
    std::string token;

    ss >> token;          // "position"
    ss >> token;

    if (token == "startpos") {
        board = Board();  
    }

    else if (token == "fen") {
        std::string fen = "";
        for (int i = 0; i < 6; i++) {
            ss >> token;
            fen += token + " ";
        }
        board.setFen(fen);
    }

    while (ss >> token) {
        if (token == "moves")
            break;
    }

    while (ss >> token) {
        Move move = uci::uciToMove(board, token);
        board.makeMove(move);
    }
}

int mobility(Board& board, Color color){
    int mob = 0;
    Bitboard occ = board.occ();

    Bitboard knights = board.pieces(PieceType::KNIGHT, color);
    while (knights) {
        Square sq = knights.pop();
        mob += attacks::knight(sq).count();
    }

    Bitboard bishops = board.pieces(PieceType::BISHOP, color);
    while (bishops) {
        Square sq = bishops.pop();
        mob += attacks::bishop(sq, occ).count();
    }

    Bitboard rooks = board.pieces(PieceType::ROOK, color);
    while (rooks) {
        Square sq = rooks.pop();
        mob += attacks::rook(sq, occ).count();
    }

    Bitboard queens = board.pieces(PieceType::QUEEN, color);
    while (queens) {
        Square sq = queens.pop();
        mob += attacks::queen(sq, occ).count();
    }

    return mob;
}

int captureThreats(const Board& board, Color color) {
    int score = 0;
    Color enemy = ~color;
    Bitboard occ = board.occ();
    Bitboard enemyPieces = board.them(color);

    // For each enemy piece, check if it's attacked by `color`
    Bitboard targets = enemyPieces;
    while (targets) {
        Square sq = targets.pop();
        if (attacks::attackers(board, color, sq)) {
            score += 20; // scaled down
        }
    }
    return score;
}

int eval(Board& board){
    int meow;
    meow += 10 * board.pieces(PieceType::PAWN, Color::WHITE).count();
    meow += 500 * board.pieces(PieceType::ROOK, Color::WHITE).count();
    meow += 310 * board.pieces(PieceType::KNIGHT, Color::WHITE).count();
    meow += 320 * board.pieces(PieceType::BISHOP, Color::WHITE).count();
    meow += 900 * board.pieces(PieceType::QUEEN, Color::WHITE).count();

    meow += 10 * board.pieces(PieceType::PAWN, Color::BLACK).count();
    meow += 500 * board.pieces(PieceType::ROOK, Color::BLACK).count();
    meow += 310 * board.pieces(PieceType::KNIGHT, Color::BLACK).count();
    meow += 320 * board.pieces(PieceType::BISHOP, Color::BLACK).count();
    meow += 900 * board.pieces(PieceType::QUEEN, Color::BLACK).count();

    meow += 5 * (mobility(board,Color(Color::WHITE)))- mobility(board,Color(Color::BLACK));

    meow +=  (board.sideToMove()!=Color(Color::WHITE) && board.inCheck() ? 200 : -200);

    meow += ((captureThreats(board, Color(Color::WHITE)) - captureThreats(board, Color(Color::BLACK)))*board.fullMoveNumber())/10;
    return meow;
}

int alphabetapruning(Board& board, int alpha, int beta, int depth, Move& bestmove,int maxdepth=7){
    Movelist moves;
    Movelist quietmoves;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board);
    movegen::legalmoves<movegen::MoveGenType::QUIET>(quietmoves, board);
    for (const auto &move : quietmoves){
        moves.add(move);
    }

    int player = int(board.sideToMove());

    if (moves.empty()){
        if (player == 0){
            if (board.inCheck()){return -10e4;}
        }//0 IS WHITE AND 1 IS BLACK
        else if (player == 1){
            if (board.inCheck()){return 10e4;}
        }
        return 0;
    }

    else if (depth == 0) {return eval(board);}
    if (player == 0){
        int maxEval = -10e5;
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
        int minEval = 10e5;
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

int main() {
    Board board;
    Move bestmove;
    string line;
    while (getline(cin, line)) {

        if (line == "uci") {
            cout << "id name MyEngine\n";
            cout << "id author Chinmaya\n";
            cout << "uciok\n";
        }

        else if (line == "isready") {
            cout << "readyok\n";
        }

        else if (line == "ucinewgame") {
            board = Board();
        }

        else if (line.substr(0,8) == "position") {
            parsePosition(board, line);
        }

        else if (line.substr(0,2) == "go") {
            alphabetapruning(board,-10e5, 10e5, 6, bestmove, 6);
            cout << "bestmove " << uci::moveToUci(bestmove) << endl;
        }

        else if (line == "quit") {
            break;
        }
    }
}