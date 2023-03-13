/**
 * FLP-project2-rubik-cube
 * Author: Matej Otcenas (xotcen01)
 * Description: Rules for parsing the input cube to internal representation
 */

% input = [
%         [[5,5,5]], Side5
%         [[5,5,5]], Side5
%		  [[5,5,5]], Side5
%         [[4,4,4],[1,1,1],[2,2,2],[3,3,3]], Side1, Side2, Side3, Side4
%         [[1,1,1],[2,2,2],[3,3,3],[4,4,4]], Side1, Side2, Side3, Side4
%         [[1,1,1],[2,2,2],[3,3,3],[4,4,4]], Side1, Side2, Side3, Side4
%         [[6,6,6]], Side6
%         [[6,6,6]], Side6
%		  [[6,6,6]]  Side6
%		  ]
% output =[
%         [5,5,5,5,5,5,5,5,5], Side5
%         [4,4,4,1,1,1,1,1,1], Side1
%         [1,1,1,2,2,2,2,2,2], Side2
%         [2,2,2,3,3,3,3,3,3], Side3
%         [3,3,3,4,4,4,4,4,4], Side4
%         [6,6,6,6,6,6,6,6,6]  Side6
%         ]
rubik_list_to_internal_rubik([[A], [B], [C], D, E, F, [G], [H], [I]], Cube) :- 
	concat_side5_6(A, B, C, Side5), 
	concat_side1_4(D, E, F, [], Side1_4), 
	concat_side5_6(G, H, I, Side6),
	append([Side5], Side1_4, Side5_1_4), append(Side5_1_4, [Side6], Cube).


% concat rows of sides 5 or 6 into one list representing one side
concat_side5_6(Row1, Row2, Row3, Result) :- append(Row1, Row2, New), append(New, Row3, Result).

concat_side1_4([], [], [], Result, Result).
concat_side1_4([Row1_1 | Rest1_1], [Row1_2 | Rest1_2], [Row1_3 | Rest1_3], Saved_concat, Result):- 
	append(Row1_1, Row1_2, Res1), append(Res1, Row1_3, Res2), append(Saved_concat, [Res2], Tmp_result),
	concat_side1_4(Rest1_1, Rest1_2, Rest1_3, Tmp_result, Result).