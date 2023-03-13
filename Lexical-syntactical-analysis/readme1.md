# Project title

This is a lexical and syntactical analysis project of code IPPCODE20.

# More detailed description

The lexical and syntactical analyser is a console based application that handles standard
input which are IPPCODE20 assembly-like instructions and creates an XML standard output 
that represents the instructions.
Parse php script is also implemented with extension 'STATP' which collects information
about parsing an IPPCODE. This script parses the arguments given to it (e.g. flags)
and behaves according to them. After the correct arguments are typed, the stdin is 
read line by line. Lexical and syntactical analyser firstly tries to find the IPP code
header (.ippcode20) and ignores comments or empty lines if there are any before header.
If the correct header is found the program continues to parse instructions, otherwise
program ends with an error. Program uses regular expressions to check the correct syntax
of each instruction. The regular expressions are mostly used to check the lexical
correctness of variables, symbols, labels and types. This program has each separate
function for these regular expressions.
When all the instructions are lexical and syntactical correct, the 
program uses the DOMDocument class to generate an xml file that is printed to stdout.
If the option of extension is chosen the program does the same steps and in addition
uses the global variables to count program statistics during the analysis. 
The matter of dispute could be considered when the string as a last parameter of
the instruction is given and without any upcoming whitespace character the comment
sign (e.g. '#') is typed. This case is handled like the following example: 
string@abcd#comment => that is parsed in this way => type=string, argument='abcd'
everything else is ignored.

## Usage

This is a console program which means that you can run it using by terminal using 
following command: php(you can specify version to 7.4) parse.php < stdin. 
You can also redirect the stdout to some file that will contain an xml content after the
redirection. This can be handy also while using test file test.php that uses these 
outputs to compare them with the correct templates.
