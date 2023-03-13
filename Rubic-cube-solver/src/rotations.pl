/**
 * FLP-project2-rubik-cube
 * Author: Matej Otcenas (xotcen01)
 * Description: Rules for hardcoded rotations of cube both clockwise and counterclockwise
 */

% cw = clockwise, ccw = counterclockwise

% rotation_up_cw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9], % Side5 Up 
        [F1, F2, F3, F4, F5, F6, F7, F8, F9], % Side1 Front
        [R1, R2, R3, R4, R5, R6, R7, R8, R9], % Side2 Right
        [B1, B2, B3, B4, B5, B6, B7, B8, B9], % Side3 Back
        [L1, L2, L3, L4, L5, L6, L7, L8, L9], % Side4 Left
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]  % Side6 Down
    ],
    [
        [U7, U4, U1, U8, U5, U2, U9, U6, U3],
        [R1, R2, R3, F4, F5, F6, F7, F8, F9],
        [B1, B2, B3, R4, R5, R6, R7, R8, R9],
        [L1, L2, L3, B4, B5, B6, B7, B8, B9],
        [F1, F2, F3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ]).

%rotation_up_ccw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U3, U6, U9, U2, U5, U8, U1, U4, U7],
        [L1, L2, L3, F4, F5, F6, F7, F8, F9],
        [F1, F2, F3, R4, R5, R6, R7, R8, R9],
        [R1, R2, R3, B4, B5, B6, B7, B8, B9],
        [B1, B2, B3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ]).

%rotation_down_cw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [L1, L2, L3, F4, F5, F6, F7, F8, F9],
        [F1, F2, F3, R4, R5, R6, R7, R8, R9],
        [R1, R2, R3, B4, B5, B6, B7, B8, B9],
        [B1, B2, B3, L4, L5, L6, L7, L8, L9],
        [D7, D4, D1, D8, D5, D2, D9, D6, D3]
    ]).

%rotation_down_ccw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, R7, R8, R9],
        [R1, R2, R3, R4, R5, R6, B7, B8, B9],
        [B1, B2, B3, B4, B5, B6, L7, L8, L9],
        [L1, L2, L3, L4, L5, L6, F7, F8, F9],
        [D3, D6, D9, D2, D5, D8, D1, D4, D7]
    ]).

%rotation_front_cw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U1, U2, U3, U4, U5, U6, L3, L6, L9],
        [F7, F4, F1, F8, F5, F2, F9, F6, F3],
        [U7, R2, R3, U8, R5, R6, U9, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, D1, L4, L5, D2, L7, L8, D3],
        [R7, R4, R1, D4, D5, D6, D7, D8, D9]
    ]).

%rotation_front_ccw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U1, U2, U3, U4, U5, U6, R1, R4, R7],
        [F3, F6, F9, F2, F5, F8, F1, F4, F7],
        [D3, R2, R3, D2, R5, R6, D1, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, U9, L4, L5, U8, L7, L8, U7],
        [L3, L6, L9, D4, D5, D6, D7, D8, D9]
    ]).
    
%rotation_right_cw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U1, U2, F3, U4, U5, F6, U7, U8, F9],
        [F1, F2, D3, F4, F5, D6, F7, F8, D9],
        [R7, R4, R1, R8, R5, R2, R9, R6, R3],
        [U9, B2, B3, U6, B5, B6, U3, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, B7, D4, D5, B4, D7, D8, B1]
    ]).

%rotation_right_ccw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [U1, U2, B7, U4, U5, B4, U7, U8, B1],
        [F1, F2, U3, F4, F5, U6, F7, F8, U9],
        [R3, R6, R9, R2, R5, R8, R1, R4, R7],
        [D9, B2, B3, D6, B5, B6, D3, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, F3, D4, D5, F6, D7, D8, F9]
    ]).

%rotation_left_cw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [B9, U2, U3, B6, U5, U6, B3, U8, U9],
        [U1, F2, F3, U4, F5, F6, U7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, D7, B4, B5, D4, B7, B8, D1],
        [L7, L4, L1, L8, L5, L2, L9, L6, L3],
        [F1, D2, D3, F4, D5, D6, F7, D8, D9]
    ]).

%rotation_left_ccw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [F1, U2, U3, F4, U5, U6, F7, U8, U9],
        [D1, F2, F3, D4, F5, F6, D7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, U7, B4, B5, U4, B7, B8, U1],
        [L3, L6, L9, L2, L5, L8, L1, L4, L7],
        [B9, D2, D3, B6, D5, D6, B3, D8, D9]
    ]).

%rotation_back_cw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [R3, R6, R9, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, D9, R4, R5, D8, R7, R8, D7],
        [B7, B4, B1, B8, B5, B2, B9, B6, B3],
        [U3, L2, L3, U2, L5, L6, U1, L8, L9],
        [D1, D2, D3, D4, D5, D6, L1, L4, L7]
    ]).

%rotation_back_ccw
rotation(
    [
        [U1, U2, U3, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, R3, R4, R5, R6, R7, R8, R9],
        [B1, B2, B3, B4, B5, B6, B7, B8, B9],
        [L1, L2, L3, L4, L5, L6, L7, L8, L9],
        [D1, D2, D3, D4, D5, D6, D7, D8, D9]
    ],
    [
        [L7, L4, L1, U4, U5, U6, U7, U8, U9],
        [F1, F2, F3, F4, F5, F6, F7, F8, F9],
        [R1, R2, U1, R4, R5, U2, R7, R8, U3],
        [B3, B6, B9, B2, B5, B8, B1, B4, B7],
        [D7, L2, L3, D8, L5, L6, D9, L8, L9],
        [D1, D2, D3, D4, D5, D6, R9, R6, R3]
    ]).