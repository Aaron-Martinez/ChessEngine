# Hoodwink
A UCI-compatible chess engine written in C++

This chess engine was created by Aaron Martinez. The initial structure of this project was based largely off of the open source chess engine Vice, with some other ideas taken from the chess programming wiki, and some individual components added on top. I haven't yet done any in-depth measure of its playing strength. For a vague idea of strength - I am rated ~1600 blitz on chess.com and have played many blitz games vs Hoodwink, and I get completely destroyed almost every time. 

I have some pgn files in example_games where I have many games of Hoodwink playing against itself. Two big things I have noticed by analyzing these games after is that the search depth is not high enough yet to catch long sequences, or tactics which are a few moves down the road. The other is king safety - many times one side or both will not castle and often get punished later. See my first personal win against Hoodwink in example_games/rapid/myFirstWin.pgn for a good example of both of these flaws in action. Fixing these are the next two immediate objectives for me.

https://www.chessprogramming.org/Main_Page

https://www.chessprogramming.org/Vice

The current code makes use of the following techniques:
* 120 index array board representation
* Pawn bitboards
* 32 bit integer move encoding
* Alpha-beta search with quiescence
* Iterative deepening
* Null move pruning
* Simple move ordering using principal variation, MVV/LVA for captures, killer and history heuristics for non captures
* Transposition table - always replace scheme
* Simple evaluation function using piece values, piece positions, and pawn structure

Moving forward I have a lot more additions I want to make. A few of those include:
* Hoodwink search mode - in losing/worse positions, search instead for more "tricky" moves, forcing opponent to find very difficult/accurate continuations in order to hold the advantage. This purpose of this idea is to find a more practical approach to escape losing positions vs human opponents, even if it means playing moves which are slightly suboptimal from a pure technical evaluation standpoint. 
* Improve the evaluation function:
    - Specific material imbalances, dynamic material values based on game phase, more sophisticated piece position and mobility scoring
    - Better metrics for king safety
    - Endgame evaluation heuristics
    - Consideration for space advantages
    - Bonuses/penalties for potential threats and weaknesses
* Parallel processing for search and evaluation
* More sophisticated move ordering
* Upgrade transposition table replacement scheme
* Improve time management and add ponder
* Implement 64 index board representation and magic bitboards
* Intensive test suite
* Possibly incorporate built-in opening book
* Possibly investigate neural network training (although that might be a whole separate project)
