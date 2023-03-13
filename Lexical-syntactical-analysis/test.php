<?php

################# Return code macros ##################
define("NO_ERROR",0);
define("MISSING_PARAM_ERROR", 10);
define("INTERNAL_ERROR", 99);
define("FILE_OPEN_ERROR", 11);

#################################### Global variabels ###########################################
$help_option = "Script (test.php in PL PHP 7.4) will automatically test parse.php and interpret.py.
It will look for given directory containing tests and use them for automatic 
testing of the correct functionality both of the programs including generation
of simple sum up in HTML 5 to standard output. 
=========================================================================
To run this script properly please follow this instruction:
php('can be added specified version 7.4 or higher') 'name_of_script.php' [--help] [--directory=path] [--recursive] [--parse-script=file] [--int-script=file] [--parse-only] [--int-only]\n";
$recursive = false;
$parse_only = false;
$int_only = false;
$directory = false;
$parse_script = false;
$int_script = false;
$jexamxml = false;
$parse_script_filename = "parse.php";  # filename is explicitly saved as parse.php in current working directory
$int_script_filename = "interpret.py";  # filename is explicitly saved as interpret.py in current working directory
$jexamxml_path = "/pub/courses/ipp/jexamxml/jexamxml.jar";  # this is the Merlin server path to jexamxml.jar


# Arguments parsing function which sets the flags to true if they were used (for further use) and returns flags
function parse_arguments()
{
    global $help_option;
    global $argc;
    global $recursive;
    global $parse_only;
    global $int_only;
    global $directory;
    global $parse_script;
    global $int_script;
    global $jexamxml;

    $longopts = ["help", "recursive", "parse-only", "int-only", "directory::", "parse-script::", "int-script::", "jexamxml::"];
    $flags = getopt("", $longopts);

    if(array_key_exists("help", $flags) && ($argc != 2))
    {
        fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
        exit(MISSING_PARAM_ERROR);
    }

    elseif(array_key_exists("help", $flags) && ($argc == 2))
    {
        fprintf(STDOUT, $help_option);
        exit(NO_ERROR);
    }
    else
    {
        foreach($flags as $flag => $value)  # finding flags
        {
            if(!strcmp($flag, "recursive")) {$recursive = true;}
            if(!strcmp($flag, "parse-only")) {$parse_only = true;}
            if(!strcmp($flag, "int-only")) {$int_only = true;}
            if(!strcmp($flag, "directory")) {$directory = true;}
            if(!strcmp($flag, "parse-script")) {$parse_script = true;}
            if(!strcmp($flag, "int-script")) {$int_script = true;}
            if(!strcmp($flag, "jexamxml")) {$jexamxml = true;}

            if(($int_only == true && $parse_only == true) || ($int_script == true && $parse_only == true))  # error, invalid flag combination
            {
                fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
                exit(MISSING_PARAM_ERROR);
            }
            elseif(($parse_only == true && $int_only == true) || ($parse_script == true && $int_only == true))  # error, invalid flag combination
            {
                fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
                exit(MISSING_PARAM_ERROR);
            }
        }
    }

    return $flags;
}

function find_directory($flags)
{
    global $directory;
    global $recursive;

    if($directory)  # find tests in a given directory path
    {
        foreach($flags as $flag => $value)
        {
            if(!strcmp($flag, "directory"))  # returning the given directory path
            {
                if(!file_exists($value) && !is_dir($value)) {exit(FILE_OPEN_ERROR);}

                if($recursive)
                {
                    $ret = find_tests_recursively($value);  # finding tests recursively
                    /*if(!(strcmp($ret, ""))) {exit(FILE_OPEN_ERROR);}  # no files were found*/
                    return $ret;
                }
                else return $value;
            }
        }
    }
    return getcwd();  # if the directory path is not given return current working directory
}

# Finding parse script name or returning implicitly set 'parse.php'
function find_filename($flags, $script_type)
{
    foreach($flags as $flag => $value)
    {
        if(!strcmp($script_type, "parser"))
        {
            if(!strcmp($flag, "parse-script")) {return $value;}  # returning the given parse script name
        }
        else if(!strcmp($flag, "int-script")) {return $value;}  # returning the given interpret script name
    }
    return INTERNAL_ERROR;
}

# Finding path to A7Soft JExamXML jar package folder
function find_jar_package($flags)
{
    foreach($flags as $flag => $value)
    {
        if(!strcmp($flag, "jexamxml")) {return $value;}  # returns the path to jar package folder
    }
    return INTERNAL_ERROR;
}

# Using shell terminal command for recursive find of the given directory
function find_tests_recursively($filename)
{
    exec("find $filename", $output, $ret_val);
    if($ret_val == "0")
    {
        return $output[0];
    }
    exit(FILE_OPEN_ERROR);
}


function parser_only_tests($path, $parse_script_filename, $jexamxml_path, $src, $rc, $out)
{
    exec("mkdir \"$path/parser_tmp_out\"");

    global $recursive;
    $src_basename = array();
    $rc_basename = array();
    $out_basename = array();

    $dir_src = array();
    $dir_rc = array();
    $dir_out = array();

    for($i = 0; $i < sizeof($src); $i++) $dir_src[$i] = dirname($src[$i]);
    for($i = 0; $i < sizeof($rc); $i++)
    {
        $dir_rc[$i] = dirname($rc[$i]);
        if (!(strcmp($path, $rc[$i]))) $dir_rc[$i] = $dir_src[$i];
    }
    for($i = 0; $i < sizeof($out); $i++)
    {
        $dir_out[$i] = dirname($out[$i]);
        if(!(strcmp($path, $out[$i]))) $dir_out[$i] = $dir_src[$i];
    }

    sort($dir_src);
    sort($dir_rc);
    sort($dir_out);

    if(!$recursive)
    {
        for ($i = 0; $i < sizeof($rc); $i++) $dir_rc[$i] = "./" . $dir_rc[$i];
        for ($i = 0; $i < sizeof($out); $i++) $dir_out[$i] = "./" . $dir_out[$i];
    }


    for($i = 0; $i < sizeof($src); $i++)
    {
        $src_basename[$i] = basename("$src[$i]", ".src");
    }

    for($i = 0; $i < sizeof($rc); $i++)
    {
        $rc_basename[$i] = basename("$rc[$i]", ".rc");
    }


    for($i = 0; $i < sizeof($out); $i++)
    {
        $out_basename[$i] = basename("$out[$i]", ".out");
    }

    for($i = 0; $i < sizeof($src); $i++)
    {
        if(sizeof($rc) <= $i) $dir_rc[$i] = $dir_rc[0];
        if(sizeof($out) <= $i) $dir_out[$i] = $dir_out[0];

        if(!in_array($src_basename[$i], $rc_basename))
        {
            if(!$recursive)
            {
                exec("touch \"$dir_src[$i]/$src_basename[$i].rc\"");
                exec("echo \"0\" > \"$dir_src[$i]/$src_basename[$i].rc\"");
            }
            else {
                exec("touch \"$dir_rc[$i]/$src_basename[$i].rc\"");
                exec("echo \"0\" > \"$dir_rc[$i]/$src_basename[$i].rc\"");
            }
        }
        if(!in_array($src_basename[$i], $out_basename))
        {
            if(!$recursive) exec("touch \"$dir_src[$i]/$src_basename[$i].out\"");
            else exec("touch \"$dir_out[$i]/$src_basename[$i].out\"");
        }
    }



    sort($dir_out);
    sort($dir_rc);

    $rc = find_rc($path);
    $out = find_out($path);

    $rc = array_filter($rc);
    $out = array_filter($out);

    sort($rc);
    sort($out);


    global $parse_only;
    $passed_tests = array();
    $failed_tests = array();
    $diff_jexamxml_ret_val = array();
    $tmp_ret_val = array();
    $rc_ret_val = array();

    for($i = 0; $i < sizeof($src); $i++)
    {
        exec("php $parse_script_filename < $src[$i] > $path/parser_tmp_out/tmp$i.tmp", $bar, $tmp_ret_val[$i]);
        if($recursive) exec("cat \"$dir_rc[$i]/$src_basename[$i].rc\"", $rc_ret_val);
        else exec("cat \"$dir_src[$i]/$src_basename[$i].rc\"", $rc_ret_val);

        if(!strcmp($tmp_ret_val[$i], $rc_ret_val[$i]) && $rc_ret_val[$i] != "0") {array_push($passed_tests, $out[$i]);}

        else{
            if($parse_only) exec("java -jar $jexamxml_path $path/parser_tmp_out/tmp$i.tmp $out[$i]",$bar, $diff_jexamxml_ret_val[$i]);
            else exec("diff $path/parser_tmp_out/tmp$i.tmp $out[$i]",$bar, $diff_jexamxml_ret_val[$i]);

            if($diff_jexamxml_ret_val[$i] == "0") {array_push($passed_tests, $out[$i]);}
            else array_push($failed_tests, $out[$i]);
        }
    }

    $tests['passed'] = $passed_tests;
    $tests['failed'] = $failed_tests;

    exec("rm -r \"$path/parser_tmp_out\"");

    return $tests;
}

function find_src($path)
{
    global $recursive;

    if($recursive)  exec("find $path -name \"*.src\"", $src_arr);
    else exec("find $path -maxdepth 1 -name \"*.src\"", $src_arr);

    if(is_array($src_arr)){
        if(empty($src_arr)) exit(FILE_OPEN_ERROR);
        sort($src_arr);
    }
    else exit(FILE_OPEN_ERROR);
    return $src_arr;
}

function find_rc($path)
{
    global $recursive;

    if($recursive) $rc_arr = shell_exec("find $path -name \"*.rc\"");
    else $rc_arr = shell_exec("find $path -maxdepth 1 -name \".*rc\"");

    if($rc_arr == "") return explode("\n", $path);
    else return explode("\n", $rc_arr);
}

function find_in($path)
{
    global $recursive;
    if($recursive) $in_arr = shell_exec("find $path -name \"*.in\"");
    else $in_arr = shell_exec("find $path -maxdepth 1 -name \"*.in\"");

    if($in_arr == "") return explode("\n", $path);
    else return $in_arr = explode("\n", $in_arr);
}

function find_out($path)
{
    global $recursive;

    if($recursive) $out_arr = shell_exec("find $path -name \"*.out\"");
    else $out_arr = shell_exec("find $path -maxdepth 1 -name \"*.out\"");

    if($out_arr == "") return explode("\n", $path);
    else return $out_arr = explode("\n", $out_arr);
}

# Argument parsing
$flags = parse_arguments();

# Finding the path to tests
$path = find_directory($flags);

$src = find_src($path);
$rc = find_rc($path);
$out = find_out($path);

$rc = array_filter($rc);
$out = array_filter($out);

sort($src);
sort($rc);
sort($out);

# Finding the name of parsing/interpret script if there is some different option given
if($parse_script) {$parse_script_filename = find_filename($flags, "parser");}
if($int_script) {$int_script_filename = find_filename($flags, "interpret");}

# Finding the name of jar package folder with tool A7Soft JExamXML
if($jexamxml) {$jexamxml_path = find_jar_package($flags);}

$tests = parser_only_tests($path, $parse_script_filename, $jexamxml_path, $src, $rc, $out);

$tests_count = sizeof($tests["passed"]) + sizeof($tests["failed"]);


echo "<!DOCTYPE html>
<html lang=\"en\">
<head>
    <link rel=\"icon\" type=\"image/png\" href=\"images/icon.png\" />
    <meta charset=\"UTF-8\">
    <title>IppTests</title>
    <style>
        * {
            font-family: Comfortaa;
            box-sizing: border-box;
        }
        
        html {
            background: black
        }
        
        img {
            width:20px;
            height: auto;
        }
        
        h1 {
            animation: header1 5s;
            animation-timing-function: ease;
            animation-fill-mode: forwards;
            position: relative;
            text-align: center;
            font-size: 50px;
            background: -webkit-linear-gradient(#33ccff, #ff33cc);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            font-style: italic;
        }
        
        @keyframes header1 {
            from {right: -2000px;}
            to {right: 0px;}
        }
        
        
        table {
            /*animation-iteration-count: 1;*/
            animation: results 5s;
            animation-timing-function: ease;
            border: 1px solid #792396;
            background-color: #555555;
            width: 50%;
            text-align: center;
            border-radius: 10px;
            margin: auto;
            animation-fill-mode: forwards;
            position: relative;
        }
        
        @keyframes results {
            from {left: -2000px;}
            to {left: 0px;}
        }
        
        table th, td {
            padding: 0px 10px;
            font-size: 14px;
        }
        
        table td, table th {
            border: 1px solid #555555;
            padding: 5px 2px;
        }
        
        table tbody td {
            font-size: 13px;
            font-weight: bold;
            color: #FFFFFF;
        }
            table tr:nth-child(even) {
            background: #CC3BFC;
        }
        
        table thead {
            background: #792396;
            border-bottom: 1px solid #050606;
        }
        
        table thead th {
            font-size: 19px;
            font-weight: bold;
            color: #FFFFFF;
            text-align: center;
        }
        
        table thead th:first-child {
            border-left: none;
        }
        
        table tfoot {
            font-size: 13px;
            font-weight: bold;
            color: #FFFFFF;
            background: #CE3CFF;
            background: -moz-linear-gradient(top, #da6dff 0%, #d34fff 66%, #CE3CFF 100%);
            background: -webkit-linear-gradient(top, #da6dff 0%, #d34fff 66%, #CE3CFF 100%);
            background: linear-gradient(to bottom, #da6dff 0%, #d34fff 66%, #CE3CFF 100%);
            border-top: 5px solid #792396;
        }
        
        table tfoot td {
            font-size: 13px;
        }
        
        h2.passed {
            color: chartreuse;
            animation: passed 2s ease-in-out;
            /*animation-iteration-count: 1;*/
            font-size: 40px;
            font-weight: bold;
            text-align: center;
            font-family: Arial;
            position: relative;
        }
        
        h2.failed {
            color: red;
            animation: failed 2s ease-in-out;
            /*animation-iteration-count: 1;*/
            font-size: 40px;
            font-weight: bold;
            text-align: center;
            font-family: Arial;
            position: relative;
        }
        
        @keyframes passed {
            0% {
                opacity: 0;
            }
            50% {
                opacity: 0.5;
                text-shadow: 0 15px 40px #050606;
            }
            100% {
                opacity: 1;
            }
        }
        
        @keyframes failed {
            0% {
                opacity: 0;
            }
            50% {
                opacity: 0.5;
                text-shadow: 0 15px 40px #050606;
            }
            100% {
                opacity: 1;
            }
        }
    </style>
</head>
<body>
    <h1><b>IPP TESTS 2020</b></h1>
    <table class='table'>
        <thead>
            <tr>
                <th colspan='2'>Tests</th>
                <th colspan='2'>Success</th>
            </tr>
        </thead>
        <tbody>
";
$j = 0;
for($i = 0; $i < $tests_count; $i++)
{
    if(sizeof($tests["passed"]) > $i)
    {
        echo "
            <tr>
                <td colspan='2'>".$tests["passed"][$i]."</td>
                <div class='table_image'>
                    <td colspan='2'><img src=\"./images/tick.png\"></td>    
                </div>
            </tr>
        ";
    }
    else{
        echo "
            <tr>
                <td colspan='2'>".$tests["failed"][$j]."</td>
                <div class='table_image'>
                    <td colspan='2'><img src=\"./images/close.png\"></td>    
                </div>
            </tr>
        ";
        $j++;
    }
}
echo "
    </tbody>
    <tfoot>
        <tr>
            <td rowspan='3'>Total</td>
        </tr>
        <tr>
            <td colspan='1'>Passed<img src=\"./images/tick.png\"></td>
             <td>".sizeof($tests["passed"])."</td>
        </tr>
        <tr>
            <td colspan='1'>Failed<img src=\"./images/close.png\"></td>
            <td>".sizeof($tests["failed"])."</td>
        </tr>
    </tfoot>
    </table>
";
    if(sizeof($tests["failed"]) == 0)
    {
        echo "
        <h2 class='passed'>All tests passed</h2>
        ";
    }
    else{
        echo "
        <h2 class='failed'>Warning: Some tests did not pass</h2>
        ";
    }
echo "
</body>
</html>
";
