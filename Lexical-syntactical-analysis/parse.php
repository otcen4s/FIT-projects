<?php
error_reporting(E_WARNING); # ignoring warning messages

################# Return code macros ##################
define("NO_ERROR",0);
define("INTERNAL_ERROR",99);
define("HEADER_ERROR", 21);
define("OPCODE_ERROR", 22);
define("LEX_SYNT_ERROR", 23);
define("MISSING_PARAM_ERROR", 10);
define("INPUT_ERROR", 11);

############################ Global variables ##################################
$help_option = "Script of type filter (parse.php in PL PHP 7.4) reads from standard input
source code in IPPcode20, checks lexical and syntactical correctness of
code and prints to standard output an XML representation of program.
=========================================================================
To run this script properly please follow this instruction:
php('can be added specified version 7.4 or higher') 'name_of_script.php' <'stdin_file_name'\n";
$instruct_order = 1;
$arg_count = 1;
$arg_type = array("int"=>0, "bool"=>1, "string"=>2, "nil"=>3);
$comments_count = 0;
$comments = false;
$stats = false;
$loc = false;
$loc_count = 0;
$labels = false;
$labels_count = 0;
$labels_arr = array();
$jumps = false;
$jumps_count = 0;
$opcode_instructions = array(
    array("CREATEFRAME", 0),
    array("PUSHFRAME", 0),
    array("POPFRAME", 0),
    array("RETURN", 0),
    array("BREAK", 0),
    array("DEFVAR", 1, "var"),
    array("CALL", 1, "label"),
    array("PUSHS", 1, "symb"),
    array("POPS", 1, "var"),
    array("WRITE", 1, "symb"),
    array("LABEL", 1, "label"),
    array("JUMP", 1, "label"),
    array("EXIT", 1, "symb"),
    array("DPRINT", 1, "symb"),
    array("MOVE", 2, "var", "symb"),
    array("INT2CHAR", 2, "var", "symb"),
    array("STRLEN", 2, "var", "symb"),
    array("READ", 2, "var", "type"),
    array("TYPE", 2, "var", "symb"),
    array("NOT", 2, "var", "symb"),
    array("ADD", 3, "var", "symb", "symb"),
    array("SUB", 3, "var", "symb", "symb"),
    array("MUL", 3, "var", "symb", "symb"),
    array("IDIV", 3, "var", "symb", "symb"),
    array("AND", 3, "var", "symb", "symb"),
    array("OR", 3, "var", "symb", "symb"),
    array("LT", 3, "var", "symb", "symb"),
    array("GT", 3, "var", "symb", "symb"),
    array("EQ", 3, "var", "symb", "symb"),
    array("STRI2INT", 3, "var", "symb", "symb"),
    array("GETCHAR", 3, "var", "symb", "symb"),
    array("SETCHAR", 3, "var", "symb", "symb"),
    array("CONCAT", 3, "var", "symb", "symb"),
    array("JUMPIFEQ", 3, "label", "symb", "symb"),
    array("JUMPIFNEQ", 3, "label", "symb", "symb")
);

################################## Functions ###################################
function check_args()
{
    global $help_option;
    global $loc;
    global $comments;
    global $labels;
    global $jumps;
    global $stats;

    $options = getopt("", ["help", "stats::", "loc", "comments", "labels", "jumps"]);

    $args_count = 0;
    foreach($options as $option => $value)
    {
        if(!strcmp($option, "stats")) {$stats = true; $args_count++;}
        elseif(!strcmp($option, "loc")) {$loc = true; $args_count++;}
        elseif(!strcmp($option, "comments")) {$comments = true; $args_count++;}
        elseif(!strcmp($option, "labels")) {$labels = true; $args_count++;}
        elseif(!strcmp($option, "jumps")) {$jumps = true; $args_count++;}
        elseif(!strcmp($option, "help")) {$args_count++;}
    }

    # If there is a correct match in array with flag 'help' then help option will be printed to stdout.
    if(array_key_exists("help", $options) && ($args_count != 1))
    {
        fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
        exit(MISSING_PARAM_ERROR);
    }

    elseif(array_key_exists("help", $options) && ($args_count == 1))
    {
        fprintf(STDOUT, $help_option);
        exit(NO_ERROR);
    }

    if(($loc || $comments || $labels || $jumps) && ($stats == false))
    {
        fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
        exit(MISSING_PARAM_ERROR);
    }

    return $options;
}

function check_header()
{
    global $read_line;
    global $comments_count;

    $read_line = strtoupper($read_line);

    if((strcmp($read_line, ".IPPCODE20")) == 0) return NO_ERROR;

    else
    {
        if(substr_compare($read_line, ".IPPCODE20#", 0, strlen(".IPPCODE20#")) == 0)
        {
            $comments_count++;
            return NO_ERROR;
        }
        exit(HEADER_ERROR);
    }
}

function find_first_comment()
{
    global $read_line;
    global $comments_count;

    $read_line = preg_replace('/\s+/', '', $read_line); # removes all whitespace characters

    # fixation required when 'read_line' contains NULL
    if(ord($read_line) == 0) return NO_ERROR;

    # this statement means there is a wrong format given
    if($read_line[0] != "#" && $read_line[0] != ".") exit(HEADER_ERROR);

    elseif($read_line[0] == ".") return NO_ERROR;

    else
    {
        # comment found
        $comments_count++;
        return INTERNAL_ERROR;
    }
}

function find_comment($line)
{
    global $comments_count;

    $line = preg_replace('/\s+/', '', $line);

    if(preg_match('/^#{1}/', $line))
    {
        $comments_count++;
        return true;
    }

    else return false;
}

function generate_instruction($opcode_label)
{
    global $instruct_order;
    global $xml_out;

    $brother = $xml_out->addChild("instruction");
    $brother->addAttribute("order", "$instruct_order");
    $brother->addAttribute("opcode", "$opcode_label");

    $instruct_order++;

    return $brother;
}

function compare_check_types($splitted_line, $found_ins, $row, $expect_args, $brother)
{
    global $arg_type;
    global $arg_count;
    global $labels_arr;
    global $labels_count;
    $index = 1;

    for ($i = 1; $i <= $expect_args; $i++)
    {
        if(!strcmp($found_ins[$row][$index + $i], "var"))
        {
            check_variable($splitted_line[$i]);

            $sister = $brother->addChild("arg$arg_count", htmlspecialchars($splitted_line[$i]));
            $sister->addAttribute("type", "var");

            $arg_count++;
        }

        elseif(!strcmp($found_ins[$row][$index + $i], "label"))
        {
            check_label($splitted_line[$i]);

            $found = false;
            for($j = 0; $j < sizeof($labels_arr); $j++)
            {
                if(strcmp($labels_arr[$j], $splitted_line[$i]) == 0)
                {
                    $found = true;
                    break;
                }
            }

            if(!$found)
            {
                $labels_count++;
                $labels_arr[count($labels_arr)] = $splitted_line[$i];
            }

            $sister = $brother->addChild("arg$arg_count", htmlspecialchars($splitted_line[$i]));
            $sister->addAttribute("type", "label");

            $arg_count++;
        }

        elseif(!strcmp($found_ins[$row][$index + $i], "symb"))
        {
            check_symbol($splitted_line[$i]);

            $exploded_arr = explode("@", $splitted_line[$i]);

            if(array_key_exists($exploded_arr[0], $arg_type))
            {
                $sister = $brother->addChild("arg$arg_count", htmlspecialchars($exploded_arr[1]));
                $sister->addAttribute("type", "$exploded_arr[0]");
            }

            else
            {
                $sister = $brother->addChild("arg$arg_count", htmlspecialchars($splitted_line[$i]));
                $sister->addAttribute("type", "var");
            }

            $arg_count++;
        }

        elseif(!strcmp($found_ins[$row][$index + $i], "type"))
        {
            check_type($splitted_line[$i]);

            $sister = $brother->addChild("arg$arg_count", "$splitted_line[$i]");
            $sister->addAttribute("type", "type");

            $arg_count++;
        }

        else exit(INTERNAL_ERROR);
    }

    if(!find_comment($splitted_line[$expect_args + 1]))
    {
        $splitted_line[$expect_args + 1] = preg_replace('/^\s+/', '', $splitted_line[$expect_args + 1]); # replacement of whitespace chars in front of a string pattern

        if(ord($splitted_line[$expect_args + 1]) != 0) exit(LEX_SYNT_ERROR);
    }

    $arg_count = 1;
}


function generate_argument($splitted_line, $expect_args, $brother)
{
    global $opcode_instructions;
    $found_ins = "";
    $row = "";

    for($i = 0; $i < sizeof($opcode_instructions); $i++)
    {
        if(!strcmp($opcode_instructions[$i][0], $splitted_line[0]))
        {
            $found_ins = $opcode_instructions;
            $row = $i;
            break;
        }
    }

    if($expect_args == 0) compare_check_types($splitted_line, $found_ins, $row, $expect_args, $brother);

    elseif($expect_args == 1) compare_check_types($splitted_line, $found_ins, $row, $expect_args, $brother);

    elseif($expect_args == 2) compare_check_types($splitted_line, $found_ins, $row, $expect_args, $brother);

    else compare_check_types($splitted_line, $found_ins, $row, $expect_args, $brother);
}

function check_variable($var)
{
    if(ord($var) == 0) exit (LEX_SYNT_ERROR);
    if(!preg_match('/^[GLT]F@([a-z]|[\Q*_-$&%?!\E])+([[:alnum:]]+|[\Q*_-$&%?!\E]*)+$/', $var)) exit(LEX_SYNT_ERROR);

    else return NO_ERROR;
}

function check_symbol($sym)
{
    if(ord($sym) == 0) exit (LEX_SYNT_ERROR);
    if(!preg_match('/(^int@[+-]?\d+$|^bool@(true|false)$|^string@((\\\[0-9]{3})*([^#\s\\\])*)*$|^nil@nil$|^[GLT]F@([[:alnum:]]+|[\Q*_-$&%?!\E]*)+$)+$/', $sym)) exit(LEX_SYNT_ERROR);

    else return NO_ERROR;
}

function check_label($lab)
{
    if(ord($lab) == 0) exit (LEX_SYNT_ERROR);
    if(!preg_match('/^([[:alnum:]]+|[\Q*_-$&%?!\E]*)+$/',$lab)) exit(LEX_SYNT_ERROR);

    else return NO_ERROR;
}

function check_type($type)
{
    if(ord($type) == 0) exit (LEX_SYNT_ERROR);
    if(!preg_match('/^(int|bool|string|nil)$/',$type)) exit(LEX_SYNT_ERROR);

    else return NO_ERROR;
}

function get_opcode($splitted_line)
{
    global $jumps_count;
    global $loc_count;

    switch($splitted_line[0])
    {
        case 'MOVE':
        case 'INT2CHAR':
        case 'READ':
        case 'STRLEN':
        case 'TYPE':
        case 'NOT':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 2, $brother);
            break;

        case 'ADD':
        case 'SUB':
        case 'MUL':
        case 'IDIV':
        case 'AND':
        case 'OR':
        case 'LT':
        case 'GT':
        case 'EQ':
        case 'STRI2INT':
        case 'CONCAT':
        case 'GETCHAR':
        case 'SETCHAR':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 3, $brother);
            break;

        case 'JUMPIFEQ':
        case 'JUMPIFNEQ':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 3, $brother);
            $jumps_count++;
            break;

        case 'CREATEFRAME':
        case 'PUSHFRAME':
        case 'POPFRAME':
        case 'BREAK':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 0, $brother);
            break;

        case 'RETURN':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 0, $brother);
            $jumps_count++;
            break;

        case 'DEFVAR':
        case 'PUSHS':
        case 'POPS':
        case 'WRITE':
        case 'LABEL':
        case 'EXIT':
        case 'DPRINT':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 1, $brother);
            break;

        case 'JUMP':
        case 'CALL':
            $loc_count++;
            $brother = generate_instruction($splitted_line[0]);
            generate_argument($splitted_line, 1, $brother);
            $jumps_count++;
            break;

        default:
            if(ord($splitted_line[1]) != 0 || ord($splitted_line[0]) != 0) exit(OPCODE_ERROR); # occurance of and unexpected token
            break;
    }
}

############################################ MAIN ############################################
# argument processing
$options = check_args();

# creating an XML object for further generating of an XML file
$dom_xml = new DOMDocument('1.0', "UTF-8");
$dom_xml->formatOutput = true;
$dom_xml->preserveWhiteSpace = false;
$root = $dom_xml->appendChild($dom_xml->createElement("program"));
$xml_out = simplexml_import_dom($dom_xml);
$xml_out->addAttribute("language", "IPPcode20");

# finding header of IPP code
$header_found = false;

while($read_line = fgets(STDIN)) # reading string by string
{
    if(!(find_first_comment()))
    {
        if(ord($read_line) == 0) continue; # empty line was read

        if(!(check_header()))
        {
            $header_found = true;
            break; # correct header found
        }
    }
}

# empty input error handle
if(!$header_found) exit(HEADER_ERROR);

# instructions processing
while($read_line = fgets(STDIN))
{
    $read_line = preg_replace('/#+/', " #", $read_line);

    if(!find_comment($read_line))
    {
        $read_line = preg_replace('/^\s+/', '', $read_line); # replacement of whitespace chars in front of a string pattern

        if(ord($read_line) == 0) continue; # empty line was read

        $splitted_line = preg_split('/\s+/', $read_line); # split string patterns and save them into indexed array

        $splitted_line[0] = strtoupper($splitted_line[0]); # opcode is case insensitive, therfore we will convert it to uppercase

        get_opcode($splitted_line);
    }
}

if($stats)
{
    foreach($options as $option => $value) { if(!strcmp($option, "stats")) $filename = $value; }

    $myfile = fopen($filename, "w") or die(12);

    foreach($options as $option => $value)
    {
        if(!strcmp($option, "loc")) fwrite($myfile, "$loc_count\n");
        elseif(!strcmp($option, "comments")) fwrite($myfile, "$comments_count\n");
        elseif(!strcmp($option, "labels")) fwrite($myfile, "$labels_count\n");
        elseif(!strcmp($option, "jumps")) fwrite($myfile, "$jumps_count\n");
    }

    fclose($myfile);
}

$dom_xml->loadXML($xml_out->asXML());
echo $dom_xml->saveXML();

?>
