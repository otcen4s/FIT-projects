/**
 * FLP-project2-rubik-cube
 * Author: Matej Otcenas (xotcen01) + BUT FIT materials
 * Description: Rules for loading stdin and parsing rubik cube into internal representation
 */

:- [utils].

% Reads line from stdin, terminates on LF or EOF.
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).

% Tests if character is EOF or LF.
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).

% split line to nested list
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    %  compatible for chars
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G is first list from nested lists

% input is list of lines
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- 
	split_lines(Ls,T), 
	split_line(L,H).


parse(Rubik_cube):-
    read_lines(L),
	split_lines(L,Rubik_list), % get nested list of input configuration
	rubik_list_to_internal_rubik(Rubik_list, Rubik_cube). % convert rubik nested list into internal representation
