-- Project: FLP DKA-2-MKA
-- Author: Matej Otčenáš (xotcen01)
-- Date: 28.2.2022

module Types where
import Data.List
import System.IO
import System.Exit
import Data.Set(Set, toList)
--import Text.Printf (printf)

type State = String
type Symbol = Char

-- Transition structure
data Transition = Transition
    { fromState :: State
    , fromSymbol :: Symbol
    , toState :: State
    } deriving (Eq, Ord)

instance Show Transition where
    show (Transition fq fs tq) = intercalate "," [fq, [fs], tq]

-- DFA internal structure
data DFA = DFA
    { states :: [State]
    , alphabet :: [Symbol]
    , start :: State
    , end :: [State]
    , transRules :: Set Transition
    } deriving (Eq)


instance Show DFA where
    show (DFA s a s0 f t) = 
        unlines $ [intercalate "," s, sort a, sort s0] ++ 
        [sort(intercalate "," f)] ++ 
        map show (toList t)

-- instance Show DFA where
--   show (DFA s a s0 f t) =
--       printf "DFA = (S, A, I, F, T)\n\
--       \States: %s\n\
--       \Alphabet: [%s]\n\
--       \Initial State: %s\n\
--       \Final States: %s\n\
--       \Transitions: \n\
--       \%s"

--       (show $ sort s) (sort a)
--       s0 (show $ sort f) (unlines (map show (toList t)))



data Action =
    JustDFA
    | MinimalDFA
    | SomethingWentWrong
    | NeedSomeHelp

data Error =
    CLIerr CLIerr
    |ParseErr ParseErr

data CLIerr =
    TooManyArgs
    | UnknownArgs
    | MissingArgs

data ParseErr =
    WrongDefinition
    |EmptyInput
    | WrongStateSet
    | WrongInitState
    | WrongFiniteState
    | WrongAlphabet
    | WrongTransitionRule

instance Show Error where
    show (CLIerr e) = show e
    show (ParseErr e) = show e

instance Show CLIerr where
    show TooManyArgs = "Too many arguments."
    show UnknownArgs = "Unknown argument."
    show MissingArgs = "Missing arguments."

instance Show ParseErr where
    show WrongDefinition = "Incorrect DFA definition."
    show EmptyInput = "Nothing to parse."
    show WrongStateSet = "Incorrect state set which can be defined only as (0, 1, 2,...) characters."
    show WrongInitState = "Initial state not defined in state set."
    show WrongFiniteState = "Finite state not defined in state set."
    show WrongAlphabet = "Alphabete can be only defined as [a-z] symbols."
    show WrongTransitionRule = "Incorrect transition rule."


-- Just error printing
err :: Error -> IO ()
err e = hPrint stderr [show e] >> exitFailure