<?php
########################################################################################################################
################################################## RETURN CODE MACROS ##################################################
########################################################################################################################

define("NO_ERROR",0);
define("MISSING_PARAM_ERROR", 10);
define("INTERNAL_ERROR", 99);
define("FILE_OPEN_ERROR", 11);

########################################################################################################################
################################################ GLOBAL VARIABLES ######################################################
########################################################################################################################
$help_option = "Script (test.php in PL PHP 7.4) will automatically test parse.php and interpret.py.
It will look for given directory containing tests and use them for automatic 
testing of the correct functionality both of the programs including generation
of simple sum up in HTML 5 to standard output. 
=========================================================================
To run this script properly please follow this instruction:
php('can be added specified version 7.4 or higher') 'name_of_script.php' [--help] [--directory=path] [--recursive] [--parse-script=file] [--int-script=file] [--parse-only] [--int-only]\n";

$options = array(
    "recursive" =>false,
    "parse_only" => false,
    "int_only" => false,
    "directory" => false,
    "parse_script" => false,
    "int_script" => false,
    "jexamxml" => false
);
$parse_script_filename = "parse.php";  # filename is explicitly saved as parse.php in current working directory
$int_script_filename = "interpret.py";  # filename is explicitly saved as interpret.py in current working directory
$jexamxml_path = "/pub/courses/ipp/jexamxml/jexamxml.jar";  # this is the Merlin server path to jexamxml.jar
$working_directory = ".";  # current working directory is set

# Error handler function for php warnings
function error_handler($errno, $errstr){
    if($errno == E_WARNING){
        print("---> ERROR WHEN OPENING DIRECTORY <---\nError number:$errno\nError message: $errstr\n");
        exit(FILE_OPEN_ERROR);  # file open error
    }
}
set_error_handler("error_handler");

########################################################################################################################
################################################## ARGUMENTS PARSING ###################################################
########################################################################################################################

# Arguments parsing function which sets the flags to true if they were used (for further use) and returns flags
function parse_arguments(){
    global $help_option;
    global $argc;
    global $options;

    $longopts = ["help", "recursive", "parse-only", "int-only", "directory:", "parse-script:", "int-script:", "jexamxml:"];
    $flags = getopt("", $longopts);

    if(array_key_exists("help", $flags) && ($argc != 2)) {
        fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
        exit(MISSING_PARAM_ERROR);
    }

    elseif(array_key_exists("help", $flags) && ($argc == 2)) {
        fprintf(STDOUT, $help_option);
        exit(NO_ERROR);
    }
    else {
        foreach($flags as $flag => $value) { # finding flags
            if(!strcmp($flag, "recursive")) $options["recursive"] = true;
            if(!strcmp($flag, "parse-only")) $options["parse_only"] = true;
            if(!strcmp($flag, "int-only")) $options["int_only"] = true;
            if(!strcmp($flag, "directory")) $options["directory"] = true;
            if(!strcmp($flag, "parse-script")) $options["parse_script"] = true;
            if(!strcmp($flag, "int-script")) $options["int_script"] = true;
            if(!strcmp($flag, "jexamxml")) $options["jexamxml"] = true;

            if(($options["int_only"] == true && $options["parse_only"] == true) || ($options["int_script"] == true && $options["parse_only"] == true)) { # error, invalid flag combination
                fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
                exit(MISSING_PARAM_ERROR);
            }
            elseif(($options["parse_only"] == true && $options["int_only"] == true) || ($options["parse_script"] == true && $options["int_only"] == true)) {# error, invalid flag combination
                fprintf(STDERR, "Wrong arguments given, please use '--help' flag.\n");
                exit(MISSING_PARAM_ERROR);
            }
        }
    }

    return $flags;
}

#################################
# Class for all tests management#
#################################
class FindAllFiles{
    public $recursion;
    public $dir;
    public $parse_only;
    public $int_only;
    public $parser_name;
    public $interpret_name;
    public $jexamxml;
    public $src_files = array();
    public $rc_files = array();
    public $out_files = array();
    public $in_files = array();
    public $parser_ret_vals = array();
    public $parser_out_files = array();
    public $interpret_out_files = array();
    public $interpret_ret_vals = array();
    public $rc_ret_vals = array();
    public $out_outputs = array();
    public $tests = array();

    public function __construct($recursive, $working_directory, $int_only, $parse_only, $parser_name, $interpret_name, $jexamxml_path){
        $this->recursion = $recursive;
        $this->dir = $working_directory;
        $this->int_only = $int_only;
        $this->parse_only = $parse_only;
        $this->parser_name = $parser_name;
        $this->interpret_name = $interpret_name;
        $this->jexamxml = $jexamxml_path;
    }

    public function run_me(){
        if($this->recursion){
          $this->find_tests_recursively();
        }
        else{
            $this->find_tests_nonrecursively();
        }
        if($this->parse_only){
            $this->start_parser();
        }
        elseif($this->int_only){
            $this->start_interpret();
        }
        else{
            $this->start_both();
        }
    }

    public function find_tests_recursively(){
        try {
            $Iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($this->dir));
        }
        catch (Exception $e){
            echo "Error: ",  $e->getMessage(), "\n";
            exit(FILE_OPEN_ERROR);
        }
        $Regex = new RegexIterator($Iterator, '/^.+\.src$/i', RecursiveRegexIterator::GET_MATCH);
        foreach ($Regex as $key => $value){
            array_push($this->src_files, $key);
        }
        $Regex = new RegexIterator($Iterator, '/^.+\.rc$/i', RecursiveRegexIterator::GET_MATCH);
        $tmp_rc = array();
        foreach ($Regex as $key => $value){
            array_push($tmp_rc, $key);
        }
        $Regex = new RegexIterator($Iterator, '/^.+\.out$/i', RecursiveRegexIterator::GET_MATCH);
        $tmp_out = array();
        foreach ($Regex as $key => $value){
            array_push($tmp_out, $key);
        }
        $Regex = new RegexIterator($Iterator, '/^.+\.in$/i', RecursiveRegexIterator::GET_MATCH);
        $tmp_in = array();
        foreach ($Regex as $key => $value){
            array_push($tmp_in, $key);
        }

        $this->search_files($tmp_rc, "rc");
        $this->search_files($tmp_out, "out");
        $this->search_files($tmp_in, "in");

        if(empty($this->src_files)){
            print("No source files (*.src) were found.\n");
            exit(FILE_OPEN_ERROR);
        }
    }

    public function find_tests_nonrecursively(){
        $this->src_files = glob($this->dir . "/*.src");
        $tmp_rc = glob($this->dir . "/*.rc");
        $tmp_out = glob($this->dir . "/*.out");
        $tmp_in = glob($this->dir . "/*.in");

        $this->search_files($tmp_rc, "rc");
        $this->search_files($tmp_out, "out");
        $this->search_files($tmp_in, "in");

        if(empty($this->src_files)){
            print("No source files (*.src) were found.\n");
            exit(FILE_OPEN_ERROR);
        }
    }

    public function search_files($tmp, $suffix){
        foreach($this->src_files as $key => $value){
            $is_there = false;
            $replaced_val = preg_replace('/\.src/', ".$suffix", $value);
            foreach($tmp as $t){  # finding in associative array
                if(!strcmp($t, $replaced_val)){
                    $is_there = true;  # no need to create new file
                    if(!strcmp($suffix, "rc")) array_push($this->rc_files, $replaced_val);
                    else if(!strcmp($suffix, "out")) array_push($this->out_files, $replaced_val);
                    else array_push($this->in_files, $replaced_val);
                    break;
                }
            }
            if(!$is_there){
                $src_basename = basename($value, '.src');  # getting source basename e.g. /path/to/dir/source.src => source
                $dir_path = dirname($value); # getting path to source directory e.g. /path/to/dir/source.src => /path/to/dir
                if(!strcmp($suffix, "rc")){
                    exec("touch $dir_path/$src_basename.$suffix && echo 0 > $dir_path/$src_basename.$suffix");
                    array_push($this->rc_files, "$dir_path/$src_basename.$suffix");
                }
                else if(!strcmp($suffix, "out")) {
                    exec("touch $dir_path/$src_basename.$suffix");
                    array_push($this->out_files, "$dir_path/$src_basename.$suffix");
                }
                else{
                    exec("touch $dir_path/$src_basename.$suffix");
                    array_push($this->in_files, "$dir_path/$src_basename.$suffix");
                }
            }
        }
    }

    public function start_parser(){
        $passed_tests = array();
        $failed_tests = array();
        foreach($this->src_files as $key => $value){
            $output = shell_exec("php $this->parser_name < $value");
            exec("php $this->parser_name < $value", $tmp, $ret_val);
            $this->parser_out_files[$value] = $output;  # output from .src file is stored to associative array
            $this->parser_ret_vals[$value] = $ret_val;  # return value from .src file is stored to associative array
        }

        $this->get_rc();
        $this->get_out();

        $rc_ret_vals_keys = array_keys($this->rc_ret_vals);
        $rc_ret_vals_values = array_values($this->rc_ret_vals);
        $out_outputs_keys = array_keys($this->out_outputs);
        $out_outputs_values = array_values($this->out_outputs);
        $parser_out_files_values = array_values($this->parser_out_files);
        $parser_ret_vals_values = array_values($this->parser_ret_vals);

        for($i = 0; $i < count($this->src_files); $i++){
            if(!strcmp($rc_ret_vals_values[$i], $parser_ret_vals_values[$i])){
                if($rc_ret_vals_values[$i] == "0"){
                    $myfile1 = fopen("out0.tmp", "w");
                    fwrite($myfile1, $parser_out_files_values[$i]);
                    $myfile2 = fopen("out1.tmp", "w");
                    fwrite($myfile2, $out_outputs_values[$i]);
                    exec("java -jar $this->jexamxml out1.tmp out0.tmp", $out, $ret_val);
                    if($ret_val == "0")  array_push($passed_tests, $out_outputs_keys[$i]);
                    else array_push($failed_tests, $out_outputs_keys[$i]);
                    unlink("./out0.tmp");
                    unlink("./out1.tmp");
                }
                else array_push($passed_tests, $rc_ret_vals_keys[$i]);
            }
            else{
                array_push($failed_tests, $rc_ret_vals_keys[$i]);
            }
        }
        $this->tests["passed"] = $passed_tests;
        $this->tests["failed"] = $failed_tests;
        #print_r($this->tests);
    }

    public function start_interpret(){
        $passed_tests = array();
        $failed_tests = array();

        $in_files_values = array_values($this->in_files);
        $in_counter = 0;
        foreach($this->src_files as $key => $value){
            $output = shell_exec("python3 $this->interpret_name --source=$value --input=$in_files_values[$in_counter]");
            exec("python3 $this->interpret_name --source=$value --input=$in_files_values[$in_counter]", $tmp, $ret_val);
            $this->interpret_out_files[$value] = $output;  # output from .src file is stored to associative array
            $this->interpret_ret_vals[$value] = $ret_val;  # return value from .src file is stored to associative array
            $in_counter++;
        }

        $this->get_rc();
        $this->get_out();

        $rc_ret_vals_keys = array_keys($this->rc_ret_vals);
        $rc_ret_vals_values = array_values($this->rc_ret_vals);
        $out_outputs_keys = array_keys($this->out_outputs);
        $out_outputs_values = array_values($this->out_outputs);
        $interpret_out_files_values = array_values($this->interpret_out_files);
        $interpret_ret_vals_values = array_values($this->interpret_ret_vals);

        for($i = 0; $i < count($this->src_files); $i++){
            if(!strcmp($rc_ret_vals_values[$i], $interpret_ret_vals_values[$i])){
                if($rc_ret_vals_values[$i] == "0"){
                    $myfile1 = fopen("out0.tmp", "w");
                    fwrite($myfile1, $interpret_out_files_values[$i]);
                    $myfile2 = fopen("out1.tmp", "w");
                    fwrite($myfile2, $out_outputs_values[$i]);
                    exec("diff out1.tmp out0.tmp", $out, $ret_val);
                    if($ret_val == "0")  array_push($passed_tests, $out_outputs_keys[$i]);
                    else array_push($failed_tests, $out_outputs_keys[$i]);
                    unlink("./out0.tmp");
                    unlink("./out1.tmp");
                }
                else array_push($passed_tests, $rc_ret_vals_keys[$i]);
            }
            else{
                array_push($failed_tests, $rc_ret_vals_keys[$i]);
            }
        }

        $this->tests["passed"] = $passed_tests;
        $this->tests["failed"] = $failed_tests;
    }

    public function start_both(){
        $passed_tests = array();
        $failed_tests = array();
        foreach($this->src_files as $key => $value){
            $output = shell_exec("php $this->parser_name < $value");
            exec("php $this->parser_name < $value", $tmp, $ret_val);
            $this->parser_out_files[$value] = $output;  # output from .src file is stored to associative array
            $this->parser_ret_vals[$value] = $ret_val;  # return value from .src file is stored to associative array
        }

        $new_src = array_values($this->parser_out_files);
        $in_files_values = array_values($this->in_files);
        $cnt = 0;
        foreach($this->src_files as $key => $value){
            $myfile1 = fopen("out0.tmp", "w");
            fwrite($myfile1, $new_src[$cnt]);
            $output = shell_exec("python3 $this->interpret_name --source=out0.tmp --input=$in_files_values[$cnt]");
            exec("python3 $this->interpret_name --source=out0.tmp --input=$in_files_values[$cnt]", $tmp, $ret_val);
            $this->interpret_out_files[$value] = $output;  # output from .src file is stored to associative array
            $this->interpret_ret_vals[$value] = $ret_val;  # return value from .src file is stored to associative array
            unlink("./out0.tmp");
            $cnt++;
        }

        $this->get_rc();
        $this->get_out();

        $rc_ret_vals_keys = array_keys($this->rc_ret_vals);
        $rc_ret_vals_values = array_values($this->rc_ret_vals);
        $out_outputs_keys = array_keys($this->out_outputs);
        $out_outputs_values = array_values($this->out_outputs);
        $interpret_out_files_values = array_values($this->interpret_out_files);
        $interpret_ret_vals_values = array_values($this->interpret_ret_vals);

        for($i = 0; $i < count($this->src_files); $i++){
            if(!strcmp($rc_ret_vals_values[$i], $interpret_ret_vals_values[$i])){
                if($rc_ret_vals_values[$i] == "0"){
                    $myfile1 = fopen("out0.tmp", "w");
                    fwrite($myfile1, $interpret_out_files_values[$i]);
                    $myfile2 = fopen("out1.tmp", "w");
                    fwrite($myfile2, $out_outputs_values[$i]);
                    exec("diff out1.tmp out0.tmp", $out, $ret_val);
                    if($ret_val == "0")  array_push($passed_tests, $out_outputs_keys[$i]);
                    else array_push($failed_tests, $out_outputs_keys[$i]);
                    unlink("./out0.tmp");
                    unlink("./out1.tmp");
                }
                else array_push($passed_tests, $rc_ret_vals_keys[$i]);
            }
            else{
                array_push($failed_tests, $rc_ret_vals_keys[$i]);
            }
        }

        $this->tests["passed"] = $passed_tests;
        $this->tests["failed"] = $failed_tests;
    }

    public function get_rc(){
        foreach($this->rc_files as $key => $value){
            $output = shell_exec("cat $value");
            $output = rtrim($output, "\ \t\n\r\0\x0B");
            $this->rc_ret_vals[$value] = $output;
        }
    }

    public function get_out(){
        foreach($this->out_files as $key => $value){
            $output = shell_exec("cat $value");
            $this->out_outputs[$value] = $output;
        }
    }
}


########################################################################################################################
###################################################### MAIN ############################################################
########################################################################################################################


$flags = parse_arguments();

if($options["parse_script"]){
    $parse_script_filename = $flags["parse-script"];
}
if($options["int_script"]){
    $int_script_filename = $flags["int-script"];
}
if($options["jexamxml"]){
    $jexamxml_path = $flags["jexamxml"];
}
if($options["directory"]){
    $working_directory = $flags["directory"];
}
if($options["recursive"]){
    $FindAllFiles = new FindAllFiles(true, $working_directory, $options["int_only"], $options["parse_only"], $parse_script_filename, $int_script_filename, $jexamxml_path);
}
else{
    $FindAllFiles = new FindAllFiles(false, $working_directory, $options["int_only"], $options["parse_only"], $parse_script_filename, $int_script_filename, $jexamxml_path);
}
$FindAllFiles->run_me();
$tests = $FindAllFiles->tests;

########################################################################################################################
###################################################### HTML FILE #######################################################
########################################################################################################################


echo ">
    <title>IppTests</title>
    <style>
        * {
            font-family: Comfortaa,serif;
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
            position: relative;
            text-align: center;
            font-size: 50px;
            background: -webkit-linear-gradient(#33ccff, #ff33cc);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            font-style: italic;
        }
        
        
        table {
            border: 1px solid #792396;
            background-color: #555555;
            width: 50%;
            text-align: center;
            border-radius: 10px;
            margin: auto;
            position: relative;
        }
       
        
        table th, td {
            padding: 0 10px;
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
            font-size: 40px;
            font-weight: bold;
            text-align: center;
            font-family: Arial,serif;
            position: relative;
        }
        
        h2.failed {
            color: red;
            font-size: 40px;
            font-weight: bold;
            text-align: center;
            font-family: Arial,serif;
            position: relative;
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
for($i = 0; $i < sizeof($tests["passed"]) + sizeof($tests["failed"]); $i++)
{
    if(sizeof($tests["passed"]) > $i)
    {
        echo "
            <tr>
                <td colspan='2'>".$tests["passed"][$i]."</td>
                <div class='table_image'>
                    <td colspan='2' style='color: lawngreen'>PASSED</td>    
                </div>
            </tr>
        ";
    }
    else{
        echo "
            <tr>
                <td colspan='2'>".$tests["failed"][$j]."</td>
                <div class='table_image'>
                    <td colspan='2' style='color: red'>FAILED</td>    
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
            <td colspan='1' style='color: lawngreen'>PASSED</td>
             <td>".sizeof($tests["passed"])."</td>
        </tr>
        <tr>
            <td colspan='1' style='color: red'>FAILED</td>
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