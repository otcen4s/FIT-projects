-- Project: FLP DKA-2-MKA
-- Author: Matej Otčenáš (xotcen01)
-- Date: 28.2.2022
module Main (main)  where
import System.Environment
import Types
import System.IO()
import Parse
import System.Exit (exitFailure)
import Minimal


printOut :: IO DFA -> IO()
printOut d = do
    dfa <- d
    putStr(show dfa)



parseMode :: String -> Types.Action
parseMode mode = case mode of
    "-t" -> Types.JustDFA
    "-i" -> Types.MinimalDFA
    "-h" -> Types.NeedSomeHelp
    _    -> Types.SomethingWentWrong


printHelp :: IO()
printHelp = do
    let helpString = "Usage: dka-2-mka (-i | -t) [inputFile]\n\
    \    -i            output internal representation of parsed DFA\n\
    \    -t            output minimized input DFA\n\
    \    [inputFile]   input file containing valid DFA. DFA is read\n\
    \                  from standard input if omitted"
    putStrLn helpString


main :: IO ()
main = do
    args <- getArgs

    case length args of -- handling arguments counts
        len | len < 1 -> Types.err (Types.CLIerr MissingArgs )
        len | len > 2 -> Types.err (Types.CLIerr TooManyArgs)
        _ -> return ()

    let (content, outputMode) = case args of
            [mode] -> (getContents, parseMode mode)
            [mode, file] -> (readFile file , parseMode mode)
            _  -> (exitFailure, Types.SomethingWentWrong)
        

    -- IO String to String conversion for further parse operations
    contentStr <- content
    let nonParsedDFA = lines contentStr

   

    if null nonParsedDFA 
        then Types.err (Types.ParseErr EmptyInput) 
    else do
        dfa <- Parse.parseDFA nonParsedDFA
        case outputMode of
            Types.JustDFA -> putStr(show dfa)
            Types.MinimalDFA -> printOut (Minimal.dfa2mdfa dfa)
            Types.NeedSomeHelp -> printHelp
            _ -> exitFailure
   




