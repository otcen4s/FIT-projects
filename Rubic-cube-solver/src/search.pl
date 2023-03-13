/**
 * FLP-project2-rubik-cube
 * Author: Matej Otcenas (xotcen01)
 * Description: Rules for modeling the iterative depth search algorithm to find the right path of rotations to solve the cube
 */

:- [rotations].
:- [print].

% check if the cube has the valid final form (solved)
valid( 
    [
        [U, U, U, U, U, U, U, U, U],
        [F, F, F, F, F, F, F, F, F],
        [R, R, R, R, R, R, R, R, R],
        [B, B, B, B, B, B, B, B, B],
        [L, L, L, L, L, L, L, L, L],
        [D, D, D, D, D, D, D, D, D]
    ]).


ids(Cube,Path) :- 
    increment_limit(Limit,1,1),
    ids1(Cube,0,Limit,Path).

ids1(Cube,Depth,Limit,[Cube]) :- 
    Depth<Limit, % check depth
    valid(Cube). % check if the last configuration is the final

ids1(Cube,Depth,Limit,[Cube|Rest]) :- 
    Depth<Limit,
    New_depth is Depth+1, % increment depth
    rotation(Cube,Rotated_cube), % change configuration -> go to another node to explore
    Rotated_cube \= Cube,
    ids1(Rotated_cube,New_depth,Limit,Rest). % recursively call IDS algorithm with the new depth and node to explore

% just increment the max depth for IDS algorithm if needed
increment_limit(Limit,Limit,_).  
increment_limit(Limit,Last_limit,Increment) :-
    New_limit is Last_limit+Increment, 
    increment_limit(Limit,New_limit,Increment).