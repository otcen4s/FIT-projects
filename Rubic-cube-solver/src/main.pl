/**
 * FLP-project2-rubik-cube
 * Author: Matej Otcenas (xotcen01)
 * Description: This module contains rules for rubik cube rotations both clockwise and counterclockwise.
 */

:- [read].
:- [print].
:- [search].


% Main
start :-
	prompt(_, ''),
    parse(Rubik_cube),
	ids(Rubik_cube, Result),
	write_cubes(Result),
	halt.