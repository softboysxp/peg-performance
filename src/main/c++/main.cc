/*
 * Copyright (c) 2010, softboysxp.com
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * A direct "translation" of the Java version for performance comparison
 */

#include <iostream>
#include <list>
#include <stack>
#include <iterator>
#include <algorithm>

#include <cstdio>
#include <sys/time.h>

using namespace std;

struct Move;

struct Coordinate {
	int row;
	int hole;
	
	Coordinate(int row, int hole) : row(row), hole(hole) {}
	
	list<Move> possibleMoves(int rowCount) const;
	
	bool operator==(const Coordinate& other) const {
		return (row == other.row && hole == other.hole);
	}
	
	bool operator!=(const Coordinate& other) const {
		return !(*this == other);
	}
};

struct Move {
	Coordinate from;
	Coordinate jumped;
	Coordinate to;
};

list<Move> Coordinate::possibleMoves(int rowCount) const {
	list<Move> moves;
	
	if (row >= 3) {
		if (hole >= 3) {
			Move new_move = {*this, Coordinate(row - 1, hole -1), Coordinate(row - 2, hole - 2)};
			moves.push_back(new_move);
		}
		
		if (row - hole >= 2) {
			Move new_move = {*this, Coordinate(row - 1, hole), Coordinate(row - 2, hole)};
			moves.push_back(new_move);
		}
	}
	
	if (hole >= 3) {
		Move new_move = {*this, Coordinate(row, hole - 1), Coordinate(row, hole - 2)};
		moves.push_back(new_move);
	}
	
	if (row - hole >= 2) {
		Move new_move = {*this, Coordinate(row, hole + 1), Coordinate(row, hole + 2)};
		moves.push_back(new_move);
	}
	
	if (rowCount - row >= 2) {
		Move new_move = {*this, Coordinate(row + 1, hole), Coordinate(row + 2, hole)};
		moves.push_back(new_move);
		
		Move new_move2 = {*this, Coordinate(row + 1, hole + 1), Coordinate(row + 2, hole + 2)};
		moves.push_back(new_move2);
	}
	
	return moves;
}

struct GameState {
	int rowCount;
	list<Coordinate> occupiedHoles;
	
	GameState(int rows, const Coordinate& emptyHole) : rowCount(rows) {
		for (int row = 1; row <= rows; row++) {
			for (int hole = 1; hole <= row; hole++) {
				Coordinate peg(row, hole);
				if (peg != emptyHole) {
					occupiedHoles.push_back(peg);
				}
			}
		}
	}
	
	GameState(const GameState& initialState, const Move& applyMe) : rowCount(initialState.rowCount), occupiedHoles(initialState.occupiedHoles) {
		occupiedHoles.remove(applyMe.from);
		occupiedHoles.remove(applyMe.jumped);
		find(occupiedHoles.begin(), occupiedHoles.end(), applyMe.to); // corresponds to the contains check in Java version
		occupiedHoles.push_back(applyMe.to);
	}
	
	list<Move> legalMoves() const {
		list<Move> legalMoves;
		for (list<Coordinate>::const_iterator c = occupiedHoles.begin(); c != occupiedHoles.end(); c++) {
			list<Move> possibleMoves = (*c).possibleMoves(rowCount);
			for (list<Move>::iterator i = possibleMoves.begin(); i != possibleMoves.end(); i++) {
				if (find(occupiedHoles.begin(), occupiedHoles.end(), (*i).jumped) != occupiedHoles.end()
					&& find(occupiedHoles.begin(), occupiedHoles.end(), (*i).to) == occupiedHoles.end()) {
					legalMoves.push_back(*i);
				}
			}
		}
		return legalMoves;
	}
	
	GameState apply(Move move) const {
		GameState new_state(*this, move);
		return new_state;
	}
	
	const long pegsRemaining() const {
		return occupiedHoles.size();
	}
};

static long gamesPlayed;
static list< stack<Move> > solutions;
static struct timeval startTime;
static struct timeval endTime;

static long diff_usec(struct timeval start, struct timeval end) {
	time_t secs = end.tv_sec - start.tv_sec;
	suseconds_t usecs = end.tv_usec - start.tv_usec;
	
	return (long) (secs * 1000000L) + usecs;
}

static void search(const GameState& gs, stack<Move>& moveStack) {
	if (gs.pegsRemaining() == 1) {
		solutions.push_back(stack<Move>(moveStack));
		gamesPlayed++;
		return;
	}
	
	list<Move> legalMoves = gs.legalMoves();
	
	if (legalMoves.empty()) {
		gamesPlayed++;
		return;
	}
	
	for (list<Move>::iterator i = legalMoves.begin(); i != legalMoves.end(); i++) {
		GameState nextState = gs.apply(*i);
		moveStack.push(*i);
		search(nextState, moveStack);
		moveStack.pop();
	}
}

int main(void) {
	gettimeofday(&startTime, NULL);
	
	GameState gs(5, Coordinate(3, 2));
	stack<Move> moves;
	
	search(gs, moves); 
	
	gettimeofday(&endTime, NULL);
	
	printf("Games played:    %6ld\n", gamesPlayed);
	printf("Solutions found: %6ld\n", solutions.size());
	printf("Time elapsed:    %6ldms\n", diff_usec(startTime, endTime) / 1000);
}

