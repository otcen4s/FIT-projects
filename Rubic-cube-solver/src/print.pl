/**
 * FLP-project2-rubik-cube
 * Author: Matej Otcenas (xotcen01)
 * Description: Rule for stdout of rubik cube
 */

write_cube(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9], % Side5 Up 
        [F1, F2, F3, F4, F5, F6, F7, F8, F9], % Side1 Front
        [R1, R2, R3, R4, R5, R6, R7, R8, R9], % Side2 Right
        [B1, B2, B3, B4, B5, B6, B7, B8, B9], % Side3 Back
        [L1, L2, L3, L4, L5, L6, L7, L8, L9], % Side4 Left
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]  % Side6 Down
    ]
    ) :-
    writef('%w%w%w\n%w%w%w\n%w%w%w\n',      [U1, U2, U3, U4, U5, U6, U7, U8, U9]),
    writef('%w%w%w %w%w%w %w%w%w %w%w%w\n', [F1, F2, F3, R1, R2, R3, B1, B2, B3, L1, L2, L3]),
    writef('%w%w%w %w%w%w %w%w%w %w%w%w\n', [F4, F5, F6, R4, R5, R6, B4, B5, B6, L4, L5, L6]),
    writef('%w%w%w %w%w%w %w%w%w %w%w%w\n', [F7, F8, F9, R7, R8, R9, B7, B8, B9, L7, L8, L9]),
    writef('%w%w%w\n%w%w%w\n%w%w%w\n\n',    [D1, D2, D3, D4, D5, D6, D7, D8, D9]).


write_cubes([Cube|[]]) :- write_cube(Cube).
write_cubes([Cube|Rest_cubes]) :- write_cube(Cube), write_cubes(Rest_cubes).