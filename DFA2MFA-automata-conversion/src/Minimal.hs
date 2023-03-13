-- Project: FLP DKA-2-MKA
-- Author: Matej Otčenáš (xotcen01)
-- Date: 28.2.2022

module Minimal where

import Types
import Parse()
import Data.Set(toList, fromList, union)
import Data.List
import Control.Monad()
import Data.Ord
import Data.Function (on)
import Data.Maybe()



dfa2mdfa :: DFA -> IO DFA
dfa2mdfa dfa = do

    let totalDFA = minimize dfa
    let minStateSet = minimizedStateSet totalDFA
    let stateSet' =  createStateSet minStateSet
    let finiteSet = createFiniteSet totalDFA stateSet'
    let initState = createInitialState totalDFA stateSet'
    let stateSet = createStateSet' stateSet'
    --let transitions =  nub (createTransitions (toList (transRules totalDFA)) stateSet')


    return DFA { -- just return DFA
                states = stateSet,
                alphabet = alphabet totalDFA,
                start = initState,
                end = finiteSet,
                transRules = transRules totalDFA}

-- Removes all unreachable states from the given DFA
-- Use nub 'trick' to avoid duplicates that would cause infinite loop
-- 1. i := 0
-- 2. Si := {q0} 
-- 3. repeat
-- 4.   Si+1 := Si ∪ {q|∃p∈Si ∃a∈Σ: δ(p,a)=q}
-- 5.   i := i + 1
-- 6. until Si = Si-1
-- 7. M′ := (Si, Σ, δ|Si, q0, F ∩ Si)
removeUnreachStates :: DFA -> [State] -> [State] -> DFA
removeUnreachStates dfa [sAct] sOld -- pattern matching for first input with only one state s0
                            | [sAct] /= sOld = removeUnreachStates dfa (nub (sAct : forAllStates [sAct] (toList $ transRules dfa))) [sAct]
                            | otherwise =  DFA { -- just return DFA
                                                states = [sAct],
                                                alphabet = alphabet dfa,
                                                start = start dfa,
                                                end = end dfa `intersect` [sAct],
                                                transRules = fromList $ removeTransitions [sAct] (toList $ transRules dfa)}
removeUnreachStates dfa (sAct:newStates) sOld
                        | (sAct:newStates) /= sOld = removeUnreachStates dfa (nub((sAct:newStates) ++ forAllStates newStates (toList $ transRules dfa))) (sAct:newStates)
                        | otherwise = DFA { -- just return DFA
                                                states = sAct:newStates,
                                                alphabet = alphabet dfa,
                                                start = start dfa,
                                                end = end dfa `intersect` (sAct:newStates),
                                                transRules = fromList $ removeTransitions (sAct:newStates) (toList $ transRules dfa)}
removeUnreachStates dfa [] _ =  dfa

-- Iterate all states from the given [State] set
forAllStates :: [State] -> [Transition] -> [State]
forAllStates [s] t = getState s t
forAllStates (s:sx) t = getState s t ++ forAllStates sx t
forAllStates [] _ = []

-- Get all 'toStates' from the given 'fromState' of all transition set 
getState :: State -> [Transition] -> [State]
getState s [t]
            | s == fromState t = [toState t] -- return destination state for this existing transition
            | otherwise = []
getState s (t:ts)
            | s == fromState t = toState t : getState s ts -- return destination state for this existing transition
            | otherwise = getState s ts
getState _ [] = []

-- Remove transitions that are no longer valid after detection of unreachable states
removeTransitions :: [State] -> [Transition] -> [Transition]
removeTransitions s [t]
                    | fromState t `elem` s && toState t `elem` s = [t]
                    | otherwise = []
removeTransitions s (t:ts)
                    | fromState t `elem` s && toState t `elem` s = t : removeTransitions s ts
                    | otherwise = removeTransitions s ts
removeTransitions _ [] = []



-- return DFA with SINK state
createTotalTransition :: DFA -> DFA
createTotalTransition dfa = DFA { -- just return DFA
                        states = states dfa ++  checkFullyDefined sinkTransitions,
                        alphabet = alphabet dfa,
                        start = start dfa,
                        end = end dfa,
                        transRules = Data.Set.union (transRules dfa) (fromList sinkTransitions)}
                    where sinkTransitions = iterateStates' (states dfa) (toList $ transRules dfa) (alphabet dfa)


createStateSet' :: [(Int, [State])] -> [State]
createStateSet' [q] = [show (fst q)]
createStateSet' (q:qs) = show (fst q) : createStateSet' qs
createStateSet' [] = []


createStateSet ::  [[State]] -> [(Int, [State])]
createStateSet = zip [0..]

createFiniteSet:: DFA -> [(Int, [State])] -> [State]
createFiniteSet dfa [q]
                    | (end dfa `intersect` snd q) /= [] = [show (fst q)]
                    | otherwise = []
createFiniteSet dfa (q:qs)
                    | (end dfa `intersect` snd q) /= [] = show (fst q) : createFiniteSet dfa qs
                    | otherwise = createFiniteSet dfa qs
createFiniteSet _ [] = []

createTransitions:: [Transition] -> [(Int, [State])] -> [Transition]
createTransitions t [q] = substituteStates t (snd q) (show (fst q))
createTransitions t (q:qs) = (substituteStates t (snd q) (show (fst q))) ++ createTransitions t qs
createTransitions _ [] = []



substituteStates :: [Transition] -> [State] -> State -> [Transition]
substituteStates [t] oldStates newState
                    | (fromState t `elem` oldStates) && (toState t  `notElem` oldStates) =  [(Transition{fromState=newState, fromSymbol=fromSymbol t, toState=toState t})]
                    | (fromState t  `notElem` oldStates) && (toState t `elem` oldStates) =  [(Transition{fromState=fromState t, fromSymbol=fromSymbol t, toState=newState})]
                    | (fromState t `elem` oldStates) && (toState t `elem` oldStates) =  [(Transition{fromState=newState, fromSymbol=fromSymbol t, toState=newState})]
                    | otherwise =  [(Transition{fromState=fromState t, fromSymbol=fromSymbol t, toState=toState t})]
substituteStates (t:ts) oldStates newState
                    | (fromState t `elem` oldStates) && (toState t  `notElem` oldStates) =  [(Transition{fromState=newState, fromSymbol=fromSymbol t, toState=toState t})] ++ substituteStates ts oldStates newState
                    | (fromState t  `notElem` oldStates) && (toState t `elem` oldStates) =  [(Transition{fromState=fromState t, fromSymbol=fromSymbol t, toState=newState})] ++ substituteStates ts oldStates newState
                    | (fromState t `elem` oldStates) && (toState t `elem` oldStates) =  [(Transition{fromState=newState, fromSymbol=fromSymbol t, toState=newState})] ++ substituteStates ts oldStates newState
                    | otherwise = substituteStates ts oldStates newState
substituteStates [] _ _ = []


createInitialState:: DFA -> [(Int, [State])] -> State
createInitialState dfa [q]
                    | start dfa `elem` snd q = show (fst q)
                    | otherwise = []
createInitialState dfa (q:qs)
                    | start dfa `elem` snd q = show (fst q)
                    | otherwise = createInitialState dfa qs
createInitialState _ [] = []

-- 1. Remove unreachable states. 
-- 2. i := 0
-- 3. ≡(0) := {(p,q)|p ∈ F ⇐⇒ q ∈ F}
-- 4. repeat
-- 5.   ≡(i+1) :={(p,q)|p ≡(i) q ∧ ∀a∈Σ: δ(p,a) ≡(i) δ(q,a)}
-- 6.   i := i + 1 
-- 7. until ≡(i) = ≡(i-1)
-- 8. Q′ := Q/ ≡(i)
-- 9. ∀p,q∈Q ∀a∈Σ: δ′([p],a)=[q] ⇔ δ(p,a)=q
-- 10. q0′ = [q0]
-- 11. F′ = {[q]|q∈F}
minimize :: DFA -> DFA
minimize dfa = DFA { -- just return DFA
                        states = states eliminatedDFA,
                        alphabet = alphabet eliminatedDFA,
                        start = start eliminatedDFA,
                        end = end eliminatedDFA,
                        transRules = transRules eliminatedDFA}
             where eliminatedDFA = removeUnreachStates fullDFA [start fullDFA] [] where fullDFA = createTotalTransition dfa

-- for each state in actualPartition do
-- get indices of toStates from (actualPartition:restPartitions) and store them as list of tuples -> [(state,[partitions])] = [(0, [0,1]), (1,[2,3]), (2,[0,1])]
-- get all first items of tuples where second item is equal and create new list of lists [[0,2], [1]] for such equal items
-- return this [[0,2], [1]] and recursively append it to newPartition
-- repeat this procedure until actualPartition != newPartition
-- returns the Q' set
minimizedStateSet :: DFA -> [[State]]
minimizedStateSet dfa = reverse $ delete [] (kDistinguishable dfa [q,f] [[]])
                    where (q,f) = zeroDistinguishable (states dfa) (end dfa)

-- do while newPartition != oldPartition
kDistinguishable :: DFA -> [[State]] -> [[State]] -> [[State]]
kDistinguishable dfa [[actualPartition]] oldPartition -- pattern matching for first input with only one state set
                            | [[actualPartition]] /= oldPartition = forEachPartition dfa [[actualPartition]]
                            | otherwise =oldPartition
kDistinguishable dfa (actualPartition:restPartitions) oldPartition
                        | (actualPartition:restPartitions) /= oldPartition = kDistinguishable dfa (forEachPartition dfa (actualPartition:restPartitions)) (actualPartition:restPartitions)
                        | otherwise = oldPartition
kDistinguishable _ [] _ = []



-- for each partition from actualPartition:restPartitions do:
forEachPartition :: DFA -> [[State]] -> [[State]]
forEachPartition dfa [[partitions]] = tuplesToList (getNewPartitions dfa [partitions] [[partitions]]) -- treba aplikovat funkciu na prevod z [([],x)] -> [[]]
forEachPartition dfa (part:rest) = tuplesToList (getNewPartitions dfa part rest) ++ forEachPartition dfa rest
forEachPartition _ _ = [[]]

-- for each state of the given partition do:
getNewPartitions :: DFA -> [State] -> [[State]] -> [([State], State)]
getNewPartitions dfa [s] otherPartitions = [(forEachToState (getToStates s (toList (transRules dfa))) otherPartitions, s)]
getNewPartitions dfa (s:sx) otherPartitions = (forEachToState (getToStates s (toList (transRules dfa))) otherPartitions, s) : getNewPartitions dfa sx otherPartitions
getNewPartitions _ [] _ = []

-- vystup bude partition indices []
-- for each toState of the given state do:
forEachToState :: [State] -> [[State]] -> [State]
forEachToState [s] partitions = getIndices (findPosition s partitions)
forEachToState (s:sx) partitions = getIndices (findPosition s partitions) ++ forEachToState sx partitions
forEachToState _ _ = []

-- convert list of tuples to final list
tuplesToList :: Ord a => [(a,b)] -> [[b]]
tuplesToList = map (map snd) . groupBy ((==) `on` fst) . sortBy (comparing fst)

-- find all toStates for given state
getToStates :: State -> [Transition] -> [State]
getToStates s [t]
                | s == fromState t = [toState t]
                | otherwise = []
getToStates s (t:ts)
                | s == fromState t = toState t : getToStates s ts
                | otherwise =  getToStates s ts
getToStates _ [] = []

-- find indices of partitions and return list of bools
findPosition ::  State -> [[State]] -> [Bool]
findPosition s [partitions] = [position s partitions]
findPosition s (p:rest) = position s p : findPosition s rest
findPosition _ [] = []

-- get indices of True character and return index as string
getIndices :: [Bool] -> [State]
getIndices p =  [show (elemIndices True p)]

-- find element 
position :: State -> [State] -> Bool
position s part =
    case s `elemIndex` part of
       Just n  -> True
       Nothing -> False

-- create k=0 distinguished sets with finite and other states
zeroDistinguishable :: [State] -> [State] -> ([State], [State])
zeroDistinguishable s f = (s \\ f, f)

-- check if there should be added sink state
checkFullyDefined :: [Transition] -> [State]
checkFullyDefined t
                    | null t = []
                    | otherwise = ["SINK"]

-- iterate all states and return their transition symbols
iterateStates :: [State] -> [Transition] -> [Symbol]
iterateStates [s] t = getSymbols s t
iterateStates (s:sx) t = getSymbols s t ++ iterateStates sx t
iterateStates [] _ = []

-- get all transition symbols for given state
getSymbols :: State -> [Transition] -> [Symbol]
getSymbols s [t]
            | s == fromState t = [fromSymbol t]
            | otherwise = []
getSymbols s (t:ts)
            | s == fromState t = fromSymbol t : getSymbols s ts
            | otherwise = getSymbols s ts
getSymbols _ [] = []


-- get the list of unused alphabet symbols
unusedSymbols :: [Symbol] -> [Symbol] -> [Symbol]
unusedSymbols stateAlpha dfaAlpha = stateAlpha \\ dfaAlpha

-- create list of new transitions with given symbol to SINK state
addTransitions :: State -> [Symbol] -> [Transition]
addTransitions s [a] = [Transition {fromState=s, fromSymbol=a, toState="SINK"}]
addTransitions s (a:as) = Transition {fromState=s, fromSymbol=a, toState="SINK"} : addTransitions s as
addTransitions _ [] = []

-- return the new list of transitions for all states
iterateStates' :: [State] -> [Transition] -> [Symbol] -> [Transition]
iterateStates' [s] t a = addTransitions s (unusedSymbols a (iterateStates [s] t))
iterateStates' (s:sx) t a =  addTransitions s (unusedSymbols a (iterateStates [s] t)) ++ iterateStates' sx t a
iterateStates' [] _ _ = []
                            