-- Project: FLP DKA-2-MKA
-- Author: Matej Otčenáš (xotcen01)
-- Date: 28.2.2022

module Parse where
import Types
import Data.Set(Set, fromList)
import System.Exit()
import Control.Monad
import System.IO()
import Data.Char
import Data.String ()
import Data.List


parseDFA :: [String] -> IO DFA
parseDFA dfa = do
    when (length dfa < 4) $ Types.err (Types.ParseErr WrongDefinition)
    let (s : a : s0 : f : t) = dfa

    let (statesOK, stateSet) = parseStateSet s
    when (False `elem` statesOK) $ Types.err (Types.ParseErr WrongStateSet)

    let (alphabetOK, symbolSet) = parseAlphabet a
    when (False `elem` alphabetOK) $ Types.err (Types.ParseErr WrongAlphabet)

    let (initStateOK, initState) = parseInitialState s0 stateSet
    when (False `elem` initStateOK) $ Types.err (Types.ParseErr WrongInitState)

    let (finiteStateSetOK, finiteSet) = parseFiniteStateSet f stateSet
    when (False `elem` finiteStateSetOK) $ Types.err (Types.ParseErr WrongFiniteState)

    let (transitionsOK, transitions) = parseTransitions t stateSet symbolSet
    when (False `elem` transitionsOK) $ Types.err (Types.ParseErr WrongTransitionRule)

    return DFA {
        states = stateSet,
        alphabet = symbolSet,
        start = initState,
        end = finiteSet,
        transRules = transitions
    }


-- Parse the initial state set 
parseStateSet :: String -> ([Bool], [String])
parseStateSet s = (map isNumber' s' ++ [checkDuplicates s'] , s') where
    s' = words' (== ',') s

-- Parse alphabet by checking that it is [a..z] chars with no duplicates
parseAlphabet :: String -> ([Bool], String)
parseAlphabet a = (map isLower a ++ [checkDuplicates' a] ++ [not (null (dropWhile isSpace a))], a)


-- Parse initial state S0 by checking the set is only one state, which is subset of state set
parseInitialState :: String -> [String] -> ([Bool], String)
parseInitialState s0 s = (allOK, s0) where
    allOK = (length s0 == 1) : [isSubset [s0] s]


parseFiniteStateSet :: String -> [String] -> ([Bool], [String])
parseFiniteStateSet f s = (allOK, f') where
    (allOK, f') = (isSubset f' s : [checkDuplicates f'], words' (== ',') f)

-- Redefinition of words function for own delimiter
words' :: (Char -> Bool) -> String -> [String]
words' p s =  case dropWhile p s of
                      "" -> []
                      s' -> w : words' p s''
                            where (w, s'') = break p s'

-- Check if the list of strings is all digit and return bool list
isNumber' :: String -> Bool
isNumber' ""  = False -- empty string number e.g non printable
isNumber' ('0':y:_) = not (isPrint y) -- check if there is only one zero character
isNumber' xs  =
  case dropWhile isDigit xs of
    ""       -> True
    _        -> False

-- check if the state or character is defined only once as it should be
checkDuplicates :: [String] -> Bool
checkDuplicates s = s == nub s

checkDuplicates' :: [Char] -> Bool
checkDuplicates' c = c == nub c

-- x is subset of y
isSubset :: [String] -> [String] -> Bool
isSubset x y = intersect x y == x

isSubset' :: [Char] -> [Char] -> Bool
isSubset' x y = intersect x y == x


-- Parse transitions by checking validity and storing them into set of transitions
parseTransitions :: [String] -> [String] -> String -> ([Bool], Set Transition)
parseTransitions t s a = (allOK, transitions) where
        allOK = checkTransitions t s a 
        transitions = fromList (storeTransitions t)


-- Store transition into list of transitions structure extracting source, destination states and symbol of alphabet 
storeTransitions :: [String] -> [Transition]
storeTransitions [t] = [Transition{fromState=head t', fromSymbol=head (t'!!1), toState=t'!!2}] where
    t' = words' (== ',') t
storeTransitions (t:ts) = Transition{fromState=head t', fromSymbol=head (t'!!1), toState=t'!!2} : storeTransitions ts where
    t' = words' (== ',') t
storeTransitions [] = []


-- Check if the transition is correct
checkTransitions :: [String] -> [String] -> String -> [Bool]
checkTransitions [t] s a  = [allOK] where
    allOK = length t' == 3
        &&  isSubset [head t'] s -- 'fromState' is subset of stateSet
        &&  isSubset [t'!!2] s -- 'toState' is subset of stateSet
        &&  isSubset' [head (t'!!1)] a  -- 'transitionSymbol' is subset of alphabet
        where t' = words' (== ',') t
checkTransitions (t:ts) s a  = allOK : checkTransitions ts s a where
    allOK = length t' == 3
        &&  isSubset [head t'] s
        &&  isSubset [t'!!2] s
        &&  isSubset' [head (t'!!1)] a 
        where t' = words' (== ',') t
checkTransitions [] _ _  = [True]

-- Create lists of states, where the transition starts
getFromStates :: [Transition] -> [State]
getFromStates [t] = [fromState t]
getFromStates (t:ts) = fromState t : getFromStates ts
getFromStates _ = []

-- Create list of states, where the transition ends
getToStates :: [Transition] -> [State]
getToStates [t] = [toState t]
getToStates (t:ts) = toState t : getToStates ts
getToStates _ = []