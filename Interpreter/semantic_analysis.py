import xml.etree.ElementTree as ET
import re
import sys

""" 
This is a class for semantic analysis of xml notation of an IPPCode20.
This class also interprets the code and is used for final output 
of interpreted program.
Methods mainly cooperate with dictionaries and lists data structures.
Main body of the analysis is the 'semantic_checker' method that is
composed of two loop. The 'while' loop iterates until the break condition
is not true, otherwise the loop continues. This loop is used for jump-like
instructions, which provides easy repetition of the instructions.
The next 'for' loop iterates over dictionary where are all the program
data including instructions, arguments, text of arguments etc.
"""


class SemanticAnalysis:

    def __init__(self, inp, input_type, source, source_type):
        self.source_type = source_type
        self.input_type = input_type
        self.source = source
        self.input = inp
        self.GF = dict()
        self.LF_stack = list()
        self.LF = None
        self.TF = None
        self.call_stack = list()  # for instructions CALL and RETURN
        self.frame_stack = list()
        self.data_stack = list()  # for instructions that operates with stack (PUSHS...)
        self.data = dict()
        self.labels = dict()
        self.jump_to = -1
        self.jump = False
        self.count_of_ins_done = 0

    def run_analysis(self):
        self.collect_data()
        if self.input_type == "file":
            self.open_input_file()
        self.find_labels()
        self.semantic_checker()

    def open_input_file(self):
        try:
            sys.stdin = open(self.input)
        except FileNotFoundError:
            exit(11)

    @staticmethod
    def read_input_file(arg_type):
        try:
            value = input()
        except EOFError:
            return "nil", "nil"
        if arg_type == "int":
            try:
                value = int(value)
                value_type = "int"
            except ValueError:
                value = "nil"  # trying to convert non integer value to integer
                value_type = "nil"

        if arg_type == "string":
            value = str(value)
            value_type = "string"
        if arg_type == "bool":
            if str(value).lower() == "true":
                value = str(value).lower()
            else:
                value = "false"
            value_type = "bool"

        return value, value_type

    """
    Function collects all data of an xml source file and stores them into
    nested dictionaries. It was created for more flexible manipulation with data.
    For access to instruction we refer to 'dict[(0..n)].
    For access to instruction argument we use 'dict[(0..n)]["ins_name"]'.
    For access to argument type we use 'dict[(0..n)]["ins_name"]["arg(0..2)"]'.
    For access to type value we use 'dict[(0..n)]["ins_name"]["arg(0..2)"]["(var, label, type, int, bool, string, nil)"]'
    """
    def collect_data(self):
        if self.source_type == "file":
            try:
                tree = ET.parse(self.source)
                root = tree.getroot()
            except ET.ParseError:
                exit(31)
        else:
            try:
                root = ET.fromstring(self.source)
            except ET.ParseError:
                exit(31)

        inst_count = 0
        for child in root:
            counter_arg = 0
            temp_dict0 = dict()
            temp_dict1 = dict()
            for sub_child in child:
                if sub_child.attrib.get("type") == "string" and sub_child.text is None:  # fixing this situation <arg0 type="string"/>
                    sub_child.text = ""
                temp_dict2 = {sub_child.attrib.get("type"): sub_child.text}
                temp_dict1["arg" + str(counter_arg)] = temp_dict2
                counter_arg += 1
            temp_dict0[child.get("opcode").upper()] = temp_dict1
            self.data[inst_count] = temp_dict0
            inst_count += 1

    def semantic_checker(self):
        while True:
            for instruction in self.data:
                if self.jump:
                    if self.jump_to == instruction:  # we have found the instruction => stop skip the instructions
                        self.jump = False
                    else:
                        continue

                if "DEFVAR" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    frame, var = self.split_var(self.data[instruction]["DEFVAR"]["arg0"]["var"])
                    if frame == "TF":
                        if self.TF is None:
                            print(f"Error: Instruction{self.data[instruction]} failed due to => access undefined TF\n", file=sys.stderr)
                            exit(55)  # trying to access undefined TF
                        if var in self.TF:
                            print(f"Error: Instruction{self.data[instruction]} failed due to => definition of already defined variable in TF\n", file=sys.stderr)
                            exit(52)  # definition of already defined variable in frame
                        self.TF[var] = "declared"
                    if frame == "LF":
                        if self.LF is None:
                            print(f"Error: Instruction{self.data[instruction]} failed due to => access undefined LF\n", file=sys.stderr)
                            exit(55)   # trying to access undefined LF
                        if var in self.LF:
                            print(f"Error: Instruction{self.data[instruction]} failed due to => definition of already defined variable in LF\n", file=sys.stderr)
                            exit(52)  # definition of already defined variable in frame
                        self.LF[var] = "declared"
                    if frame == "GF":
                        if var in self.GF:
                            print(f"Error: Instruction{self.data[instruction]} failed due to => definition of already defined variable in GF\n", file=sys.stderr)
                            exit(52)  # definition of already defined variable in frame
                        self.GF[var] = "declared"

                if "CREATEFRAME" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    self.TF = dict()  # creating temporary frame

                if "PUSHFRAME" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    if self.TF is None:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to access undefined TF\n", file=sys.stderr)
                        exit(55)  # trying to access undefined TF
                    self.frame_stack.append(self.TF)
                    self.LF = self.frame_stack[len(self.frame_stack) - 1]
                    self.TF = None  # temporary frame has to become undefined after pushing it to local frame

                if "POPFRAME" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    if not self.frame_stack:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to pop but nothing in stack\n", file=sys.stderr)
                        exit(55)
                    self.TF = self.frame_stack.pop()
                    if self.frame_stack:
                        self.LF = self.frame_stack[len(self.frame_stack) - 1]
                    else:
                        self.LF = None

                if "PUSHS" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["PUSHS"]["arg0"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["PUSHS"]["arg0"]["var"])
                        self.data_stack.append(list(self.insert_return_frame_value(frame, var, None, key_value)))
                    else:
                        self.data_stack.append([self.data[instruction]["PUSHS"]["arg0"][key_value], key_value])  # pushing values to stack

                if "POPS" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    if not self.data_stack:  # check if stack is empty
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to pop nothing\n", file=sys.stderr)
                        exit(56)
                    frame, var = self.split_var(self.data[instruction]["POPS"]["arg0"]["var"])
                    popped_value, arg_type = self.data_stack.pop()
                    self.insert_return_frame_value(frame, var, popped_value, arg_type)

                if "WRITE" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["WRITE"]["arg0"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["WRITE"]["arg0"]["var"])
                        print_out, arg_type = self.insert_return_frame_value(frame, var, None, key_value)
                        if arg_type == "nil":
                            print("", end="")
                        elif arg_type == "string":
                            print_out = self.convert_escape_sequence(print_out)
                            print(print_out, end="")
                        else:
                            print(print_out, end="")
                    elif key_value == "nil":
                        print("", end="")
                    elif key_value == "string":
                        print_out = self.convert_escape_sequence(self.data[instruction]["WRITE"]["arg0"][key_value])
                        print(print_out, end="")
                    else:
                        print(self.data[instruction]["WRITE"]["arg0"][key_value], end="")

                if "INT2CHAR" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["INT2CHAR"]["arg1"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["INT2CHAR"]["arg1"]["var"])
                        value_to_convert, arg_type = self.insert_return_frame_value(frame, var, None, key_value)  # getting value from frame
                    else:
                        value_to_convert = self.data[instruction]["INT2CHAR"]["arg1"][key_value]
                        arg_type = key_value

                    if arg_type != "int":
                        print(f"Error: Instruction{self.data[instruction]} failed due to => type not int\n", file=sys.stderr)
                        exit(53)
                    try:
                        converted_value = chr(int(value_to_convert))  # converting int to char value, e.g 32=>space
                    except ValueError:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => bad int type\n", file=sys.stderr)
                        exit(58)
                    frame, var = self.split_var(self.data[instruction]["INT2CHAR"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, converted_value, "string")  # inserting value to frame

                if "MOVE" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["MOVE"]["arg1"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["MOVE"]["arg1"]["var"])
                        value_to_move, arg_type = self.insert_return_frame_value(frame, var, None, key_value)  # getting value from frame
                    else:
                        value_to_move = self.data[instruction]["MOVE"]["arg1"][key_value]
                        arg_type = key_value
                    frame, var = self.split_var(self.data[instruction]["MOVE"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value_to_move, arg_type)  # inserting value to frame

                if "STRI2INT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["STRI2INT"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["STRI2INT"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["STRI2INT"]["arg1"]["var"])
                        str_to_convert, str_arg_type = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        str_len = len(str_to_convert) - 1
                        if str_arg_type != "string":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                            exit(53)
                    elif key_value1 == "string":
                        str_to_convert = self.data[instruction]["STRI2INT"]["arg1"][key_value1]
                        str_len = len(str_to_convert) - 1
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["STRI2INT"]["arg2"]["var"])
                        idx_to_convert, idx_arg_type = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        idx_to_convert = int(idx_to_convert)
                        if idx_arg_type != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not int\n", file=sys.stderr)
                            exit(53)
                    elif key_value2 == "int":
                        idx_to_convert = self.data[instruction]["STRI2INT"]["arg2"][key_value2]
                        idx_to_convert = int(idx_to_convert)
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not int\n", file=sys.stderr)
                        exit(53)

                    if idx_to_convert < 0 or str_len < idx_to_convert:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => index out of range\n", file=sys.stderr)
                        exit(58)  # trying to access index of string out of range

                    converted_value = ord(str_to_convert[idx_to_convert])
                    frame, var = self.split_var(self.data[instruction]["STRI2INT"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, converted_value, "int")  # inserting value to frame

                if "CONCAT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["CONCAT"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["CONCAT"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["CONCAT"]["arg1"]["var"])
                        str1_to_concat, str1_arg_type = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if str1_arg_type != "string":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                            exit(53)
                    elif key_value1 == "string":
                        str1_to_concat = self.data[instruction]["CONCAT"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["CONCAT"]["arg2"]["var"])
                        str2_to_concat, str2_arg_type = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if str2_arg_type != "string":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                            exit(53)
                    elif key_value2 == "string":
                        str2_to_concat = self.data[instruction]["CONCAT"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)

                    str_concat = str1_to_concat + str2_to_concat
                    frame, var = self.split_var(self.data[instruction]["CONCAT"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, str_concat, "string")  # inserting value to frame

                if "STRLEN" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["STRLEN"]["arg1"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["STRLEN"]["arg1"]["var"])
                        str_to_find, str_arg_type = self.insert_return_frame_value(frame, var, None, key_value)  # getting value from frame
                        if str_arg_type != "string":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                            exit(53)
                    elif key_value == "string":
                        str_to_find = self.data[instruction]["STRLEN"]["arg1"][key_value]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)

                    str_len = len(str_to_find)
                    frame, var = self.split_var(self.data[instruction]["STRLEN"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, str_len, "int")  # inserting value to frame

                if "GETCHAR" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["GETCHAR"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["GETCHAR"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["GETCHAR"]["arg1"]["var"])
                        str_to_getchar, str_arg_type = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if str_arg_type != "string":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                            exit(53)
                    elif key_value1 == "string":
                        str_to_getchar = self.data[instruction]["GETCHAR"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)

                    str_len = len(str_to_getchar) - 1

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["GETCHAR"]["arg2"]["var"])
                        idx_to_getchar, idx_arg_type = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if str2_arg_type != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not int\n", file=sys.stderr)
                            exit(53)
                    elif key_value2 == "int":
                        idx_to_getchar = self.data[instruction]["GETCHAR"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not int\n", file=sys.stderr)
                        exit(53)

                    idx_to_getchar = int(idx_to_getchar)  # type is int but everything is stored as string so we must convert to int

                    if idx_to_getchar < 0 or idx_to_getchar > str_len:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => index out of range\n", file=sys.stderr)
                        exit(58)

                    char_in_pos = str_to_getchar[idx_to_getchar]
                    frame, var = self.split_var(self.data[instruction]["GETCHAR"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, char_in_pos, "string")  # inserting value to frame

                if "SETCHAR" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["SETCHAR"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["SETCHAR"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["SETCHAR"]["arg1"]["var"])
                        idx_to_setchar, idx_arg_type = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if idx_arg_type != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => index not int\n", file=sys.stderr)
                            exit(53)
                    elif key_value1 == "int":
                        idx_to_setchar = self.data[instruction]["SETCHAR"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => index not int\n", file=sys.stderr)
                        exit(53)

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["SETCHAR"]["arg2"]["var"])
                        one_char, one_char_arg_type = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if one_char_arg_type != "string":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                            exit(53)
                    elif key_value2 == "string":
                        one_char = self.data[instruction]["SETCHAR"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)

                    one_char = self.convert_escape_sequence(one_char)

                    frame, var = self.split_var(self.data[instruction]["SETCHAR"]["arg0"]["var"])
                    var_str, var_arg_type = self.insert_return_frame_value(frame, var, None, "whatever")  # obtaining value from frame
                    if var_arg_type != "string":
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument not string\n", file=sys.stderr)
                        exit(53)
                    var_len = len(var_str) - 1

                    if len(one_char) <= 0 or int(idx_to_setchar) < 0 or int(idx_to_setchar) > var_len:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => empty string\n", file=sys.stderr)
                        exit(58)  # empty string

                    var_str = list(var_str)  # python conversion needed for accessing string character in exact positions
                    var_str[int(idx_to_setchar)] = list(one_char)[0]
                    var_str = ''.join(var_str)
                    self.insert_return_frame_value(frame, var, var_str, "string")  # inserting value to frame

                if "TYPE" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["TYPE"]["arg1"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["TYPE"]["arg1"]["var"])
                        value, arg_type = self.insert_return_frame_value(frame, var, None, "finding type")  # getting value from frame
                        if arg_type is None:
                            arg_type = ""
                    else:
                        arg_type = key_value

                    frame, var = self.split_var(self.data[instruction]["TYPE"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, arg_type, "string")  # inserting value to frame

                if "JUMP" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["JUMP"]["arg0"])
                    value = self.data[instruction]["JUMP"]["arg0"][key_value]
                    if value in self.labels:
                        value = self.labels[self.data[instruction]["JUMP"]["arg0"][key_value]]  # ins number
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to jump in non existing label\n", file=sys.stderr)
                        exit(52)  # trying to jump into non existent label
                    self.jump = True
                    self.jump_to = value
                    break

                if "JUMPIFEQ" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["JUMPIFEQ"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["JUMPIFEQ"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["JUMPIFEQ"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                    else:
                        value1 = self.data[instruction]["JUMPIFEQ"]["arg1"][key_value1]
                        arg_type1 = key_value1

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["JUMPIFEQ"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                    else:
                        value2 = self.data[instruction]["JUMPIFEQ"]["arg2"][key_value2]
                        arg_type2 = key_value2

                    if arg_type1 != arg_type2 and (arg_type1 != "nil" and arg_type2 != "nil"):
                        print(f"Error: Instruction{self.data[instruction]} failed due to => bad argument types\n", file=sys.stderr)
                        exit(53)

                    key_value = self.access_key_value(self.data[instruction]["JUMPIFEQ"]["arg0"])
                    value = self.data[instruction]["JUMPIFEQ"]["arg0"][key_value]
                    if value in self.labels:
                        value = self.labels[self.data[instruction]["JUMPIFEQ"]["arg0"][key_value]]  # ins number
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to jump in non existing label\n", file=sys.stderr)
                        exit(52)  # trying to jump into non existent label

                    if arg_type1 == "nil" or arg_type2 == "nil":
                        if arg_type1 != arg_type2:
                            continue
                    elif arg_type1 == "int":
                        if int(value1) != int(value2):
                            continue
                    elif arg_type1 == "string":
                        value1 = self.convert_escape_sequence(value1)
                        value2 = self.convert_escape_sequence(value2)
                        if value1 != value2:
                            continue
                    else:
                        if value1 != value2:
                            continue

                    self.jump = True
                    self.jump_to = value
                    break

                if "JUMPIFNEQ" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["JUMPIFNEQ"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["JUMPIFNEQ"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["JUMPIFNEQ"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                    else:
                        value1 = self.data[instruction]["JUMPIFNEQ"]["arg1"][key_value1]
                        arg_type1 = key_value1

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["JUMPIFNEQ"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None,
                                                                           key_value2)  # getting value from frame
                    else:
                        value2 = self.data[instruction]["JUMPIFNEQ"]["arg2"][key_value2]
                        arg_type2 = key_value2

                    if arg_type1 != arg_type2 and (arg_type1 != "nil" and arg_type2 != "nil"):
                        print(f"Error: Instruction{self.data[instruction]} failed due to => bad argument types\n", file=sys.stderr)
                        exit(53)

                    key_value = self.access_key_value(self.data[instruction]["JUMPIFNEQ"]["arg0"])
                    value = self.data[instruction]["JUMPIFNEQ"]["arg0"][key_value]
                    if value in self.labels:
                        value = self.labels[
                            self.data[instruction]["JUMPIFNEQ"]["arg0"][key_value]]  # ins number e.g. "ins1", "ins2"...
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to jump in non existing label\n", file=sys.stderr)
                        exit(52)  # trying to jump into non existent label

                    if arg_type1 == "nil" or arg_type2 == "nil":
                        if arg_type1 == arg_type2:
                            continue
                    elif arg_type1 == "int":
                        if int(value1) == int(value2):
                            continue
                    elif arg_type1 == "string":
                        value1 = self.convert_escape_sequence(value1)
                        value2 = self.convert_escape_sequence(value2)
                        if value1 == value2:
                            continue
                    else:
                        if value1 == value2:
                            continue

                    self.jump = True
                    self.jump_to = value
                    break

                if "EXIT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["EXIT"]["arg0"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["EXIT"]["arg0"]["var"])
                        value, arg_type = self.insert_return_frame_value(frame, var, None, key_value)  # getting value from frame
                        if arg_type != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)
                    elif key_value == "int":
                        value = self.data[instruction]["EXIT"]["arg0"][key_value]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)

                    value = int(value)
                    if value < 0 or value > 49:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => exit value out of range\n", file=sys.stderr)
                        exit(57)

                    exit(value)  # program will end with typed exit code

                if "DPRINT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["DPRINT"]["arg0"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["DPRINT"]["arg0"]["var"])
                        value, arg_type = self.insert_return_frame_value(frame, var, None, key_value)  # getting value from frame
                    else:
                        value = self.data[instruction]["DPRINT"]["arg0"][key_value]
                    print(value, file=sys.stderr)

                if "BREAK" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    print(f"Count of instructions completed: {self.count_of_ins_done}", file=sys.stderr)
                    print("Frames content:", file=sys.stderr)
                    print(f"GF ==> {self.GF}\nLF ==> {self.LF}\nTF ==> {self.TF}\nFRAME_STACK ==> {self.frame_stack}\n", file=sys.stderr)

                if "ADD" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["ADD"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["ADD"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["ADD"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if arg_type1 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value1 == "int":
                        value1 = self.data[instruction]["ADD"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["ADD"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if arg_type2 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value2 == "int":
                        value2 = self.data[instruction]["ADD"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    value = int(value1) + int(value2)
                    frame, var = self.split_var(self.data[instruction]["ADD"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "int")  # inserting value to frame

                if "SUB" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["SUB"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["SUB"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["SUB"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if arg_type1 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value1 == "int":
                        value1 = self.data[instruction]["SUB"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["SUB"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if arg_type2 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value2 == "int":
                        value2 = self.data[instruction]["SUB"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    value = int(value1) - int(value2)
                    frame, var = self.split_var(self.data[instruction]["SUB"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "int")  # inserting value to frame

                if "MUL" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["MUL"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["MUL"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["MUL"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if arg_type1 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value1 == "int":
                        value1 = self.data[instruction]["MUL"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["MUL"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if arg_type2 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value2 == "int":
                        value2 = self.data[instruction]["MUL"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    value = int(value1) * int(value2)
                    frame, var = self.split_var(self.data[instruction]["MUL"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "int")  # inserting value to frame

                if "IDIV" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["IDIV"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["IDIV"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["IDIV"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if arg_type1 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value1 == "int":
                        value1 = self.data[instruction]["IDIV"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["IDIV"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None,
                                                                           key_value2)  # getting value from frame
                        if arg_type2 != "int":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                            exit(53)  # operand must be int
                    elif key_value2 == "int":
                        value2 = self.data[instruction]["IDIV"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not int\n", file=sys.stderr)
                        exit(53)  # must be int

                    if int(value2) == 0:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => division by zero\n", file=sys.stderr)
                        exit(57)  # division by zero

                    value = int(value1) / int(value2)
                    frame, var = self.split_var(self.data[instruction]["IDIV"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, int(value), "int")  # inserting value to frame

                if "LT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["LT"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["LT"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["LT"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                    else:
                        value1 = self.data[instruction]["LT"]["arg1"][key_value1]
                        arg_type1 = key_value1

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["LT"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                    else:
                        value2 = self.data[instruction]["LT"]["arg2"][key_value2]
                        arg_type2 = key_value2

                    if arg_type1 != arg_type2 or arg_type1 == "nil" or arg_type2 == "nil":
                        print(f"Error: Instruction{self.data[instruction]} failed due to => wrong argument types\n", file=sys.stderr)
                        exit(53)

                    if arg_type1 == "int":
                        if int(value1) < int(value2):
                            value = "true"
                        else:
                            value = "false"
                    if arg_type1 == "string":
                        value1 = self.convert_escape_sequence(value1)
                        value2 = self.convert_escape_sequence(value2)
                        if value1 < value2:
                            value = "true"
                        else:
                            value = "false"
                    if arg_type1 == "bool":
                        if value1 == "false" and value2 == "true":
                            value = "true"
                        else:
                            value = "false"
                    frame, var = self.split_var(self.data[instruction]["LT"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "bool")  # inserting value to frame

                if "GT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["GT"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["GT"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["GT"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                    else:
                        value1 = self.data[instruction]["GT"]["arg1"][key_value1]
                        arg_type1 = key_value1

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["GT"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                    else:
                        value2 = self.data[instruction]["GT"]["arg2"][key_value2]
                        arg_type2 = key_value2

                    if arg_type1 != arg_type2 or arg_type1 == "nil" or arg_type2 == "nil":
                        print(f"Error: Instruction{self.data[instruction]} failed due to => wrong argument types\n", file=sys.stderr)
                        exit(53)

                    if arg_type1 == "int":
                        if int(value1) > int(value2):
                            value = "true"
                        else:
                            value = "false"
                    if arg_type1 == "string":
                        value1 = self.convert_escape_sequence(value1)
                        value2 = self.convert_escape_sequence(value2)
                        if value1 > value2:
                            value = "true"
                        else:
                            value = "false"
                    if arg_type1 == "bool":
                        if value1 == "true" and value2 == "false":
                            value = "true"
                        else:
                            value = "false"
                    frame, var = self.split_var(self.data[instruction]["GT"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "bool")  # inserting value to frame

                if "EQ" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["EQ"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["EQ"]["arg2"])

                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["EQ"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                    else:
                        value1 = self.data[instruction]["EQ"]["arg1"][key_value1]
                        arg_type1 = key_value1

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["EQ"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                    else:
                        value2 = self.data[instruction]["EQ"]["arg2"][key_value2]
                        arg_type2 = key_value2

                    if arg_type1 != arg_type2 and (arg_type1 != "nil" and arg_type2 != "nil"):
                        print(f"Error: Instruction{self.data[instruction]} failed due to => wrong argument types\n", file=sys.stderr)
                        exit(53)

                    if arg_type1 != arg_type2:
                        value = "false"

                    elif arg_type1 == "nil" or arg_type2 == "nil":
                        if arg_type1 == arg_type2:
                            value = "true"
                        else:
                            value = "false"
                    elif arg_type1 == "int":
                        if int(value1) == int(value2):
                            value = "true"
                        else:
                            value = "false"
                    elif arg_type1 == "string":
                        value1 = self.convert_escape_sequence(value1)
                        value2 = self.convert_escape_sequence(value2)
                        if value1 == value2:
                            value = "true"
                        else:
                            value = "false"
                    elif arg_type1 == "bool":
                        if (value1 == "true" and value2 == "true") or (value1 == "false" and value2 == "false"):
                            value = "true"
                        else:
                            value = "false"
                    frame, var = self.split_var(self.data[instruction]["EQ"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "bool")  # inserting value to frame

                if "AND" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["AND"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["AND"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["AND"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if arg_type1 != "bool":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                            exit(53)
                    elif key_value1 == "bool":
                        value1 = self.data[instruction]["AND"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                        exit(53)

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["AND"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if arg_type2 != "bool":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                            exit(53)
                    elif key_value2 == "bool":
                        value2 = self.data[instruction]["AND"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                        exit(53)

                    if value1 == "true" and value2 == "true":
                        value = "true"
                    else:
                        value = "false"

                    frame, var = self.split_var(self.data[instruction]["AND"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "bool")  # inserting value to frame

                if "OR" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value1 = self.access_key_value(self.data[instruction]["OR"]["arg1"])
                    key_value2 = self.access_key_value(self.data[instruction]["OR"]["arg2"])
                    if key_value1 == "var":
                        frame, var = self.split_var(self.data[instruction]["OR"]["arg1"]["var"])
                        value1, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value1)  # getting value from frame
                        if arg_type1 != "bool":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                            exit(53)
                    elif key_value1 == "bool":
                        value1 = self.data[instruction]["OR"]["arg1"][key_value1]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                        exit(53)

                    if key_value2 == "var":
                        frame, var = self.split_var(self.data[instruction]["OR"]["arg2"]["var"])
                        value2, arg_type2 = self.insert_return_frame_value(frame, var, None, key_value2)  # getting value from frame
                        if arg_type2 != "bool":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                            exit(53)
                    elif key_value2 == "bool":
                        value2 = self.data[instruction]["OR"]["arg2"][key_value2]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                        exit(53)

                    if value1 == "false" and value2 == "false":
                        value = "false"
                    else:
                        value = "true"

                    frame, var = self.split_var(self.data[instruction]["OR"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "bool")  # inserting value to frame

                if "NOT" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["NOT"]["arg1"])
                    if key_value == "var":
                        frame, var = self.split_var(self.data[instruction]["NOT"]["arg1"]["var"])
                        value, arg_type1 = self.insert_return_frame_value(frame, var, None, key_value)  # getting value from frame
                        if arg_type1 != "bool":
                            print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                            exit(53)
                    elif key_value == "bool":
                        value = self.data[instruction]["NOT"]["arg1"][key_value]
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => argument type not bool\n", file=sys.stderr)
                        exit(53)

                    if value == "false":
                        value = "true"
                    else:
                        value = "false"

                    frame, var = self.split_var(self.data[instruction]["NOT"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, "bool")  # inserting value to frame

                if "READ" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["READ"]["arg1"])
                    type_value = self.data[instruction]["READ"]["arg1"][key_value]

                    value, type_value = self.read_input_file(type_value)

                    frame, var = self.split_var(self.data[instruction]["READ"]["arg0"]["var"])
                    self.insert_return_frame_value(frame, var, value, type_value)  # inserting value to frame

                if "CALL" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    key_value = self.access_key_value(self.data[instruction]["CALL"]["arg0"])
                    value = self.data[instruction]["CALL"]["arg0"][key_value]
                    if value in self.labels:
                        value = self.labels[self.data[instruction]["CALL"]["arg0"][key_value]]  # ins number
                    else:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => trying to jump in non existent label\n", file=sys.stderr)
                        exit(52)  # trying to jump into non existent label
                    self.call_stack.append(instruction + 1)  # pushing incremented value of position to stack
                    self.jump = True
                    self.jump_to = value
                    break

                if "RETURN" in self.data[instruction]:
                    self.count_of_ins_done += 1
                    if not self.call_stack:
                        print(f"Error: Instruction{self.data[instruction]} failed due to => no label to jump back\n", file=sys.stderr)
                        exit(56)
                    self.jump = True
                    self.jump_to = self.call_stack.pop()
                    break

                if "LABEL" in self.data[instruction]:
                    self.count_of_ins_done += 1

            if not self.jump:
                break

    @staticmethod
    def split_var(var):
        var = re.split(r'@', var)
        return var[0], var[1]  # returns two  values(frame and value) e.g 'GF@var' => frame=GF value=var

    @staticmethod
    def access_key_value(argument):
        for arg in argument:
            if "string" in arg:
                return "string"
            if "int" in arg:
                return "int"
            if "bool" in arg:
                return "bool"
            if "nil" in arg:
                return "nil"
            if "var" in arg:
                return "var"
            if "label" in arg:
                return "label"
            if "type" in arg:
                return "type"

    """
    Method for looking up values in frames or inserting values into frames.
    This method also checks some error occurrences such as trying to get
    value from undefined variables and so on. 
    """

    def insert_return_frame_value(self, frame, var, value, var_type):
        if frame == "GF":
            self.check_existence(frame, var)
            if value is None:
                if self.GF[var] == "declared":
                    if var_type == "finding type":
                        return None, None
                    print(f"Error: failed due to => trying to get value from undefined variable {var} in frame {self.GF}\n", file=sys.stderr)
                    exit(56)  # trying to get value from declared but undefined variable
                return self.GF[var][0], self.GF[var][1]  # returning two values with value and type e.g ["hello", string]
            self.GF[var] = [value, var_type]  # inserting value to frame
        if frame == "LF":
            if self.LF is None:
                print(f"Error: failed due to => trying to access undefined LF\n", file=sys.stderr)
                exit(55)  # trying to access undefined LF
            self.check_existence(frame, var)
            if value is None:
                if self.LF[var] == "declared":
                    if var_type == "finding type":
                        return None, None
                    print(f"Error: failed due to => trying to get value from undefined variable {var} in frame {self.LF}\n", file=sys.stderr)
                    exit(56)  # trying to get value from declared but undefined variable
                return self.LF[var][0], self.LF[var][1]  # returning two values with value and type e.g ["hello", string]
            self.LF[var] = [value, var_type]  # inserting value to frame
        if frame == "TF":
            if self.TF is None:
                print(f"Error: failed due to => trying to access undefined TF\n", file=sys.stderr)
                exit(55)  # trying to access undefined LF
            self.check_existence(frame, var)
            if value is None:
                if self.TF[var] == "declared":
                    if var_type == "finding type":
                        return None, None
                    print(f"Error: failed due to => trying to get value from undefined variable {var} in frame {self.TF}\n", file=sys.stderr)
                    exit(56)  # trying to get value from declared but undefined variable
                return self.TF[var][0], self.TF[var][1]  # returning two values with value and type e.g ["hello", string]
            self.TF[var] = [value, var_type]  # inserting value to frame

    """
    Method checks if the variable was declared in frame.
    """

    def check_existence(self, frame, var):
        if frame == "TF":
            if var not in self.TF:
                print(f"Error: failed due to => trying to get undeclared variable {var} from frame {self.TF}\n", file=sys.stderr)
                exit(54)
        if frame == "LF":
            if var not in self.LF:
                print(f"Error: failed due to => trying to get undeclared variable {var} from frame {self.LF}\n", file=sys.stderr)
                exit(54)
        if frame == "GF":
            if var not in self.GF:
                print(f"Error: failed due to => trying to get undeclared variable {var} from frame {self.GF}\n", file=sys.stderr)
                exit(54)

    """
    Finding all labels in code if there are any and storing them into dictionary.
    E.g. labels{
            while: ins3
            here: ins7
            ...
    """
    def find_labels(self):
        for instruction in self.data:
            if "LABEL" in self.data[instruction]:
                key_value = self.access_key_value(self.data[instruction]["LABEL"]["arg0"])
                value = self.data[instruction]["LABEL"]["arg0"][key_value]
                if value in self.labels:
                    print(f"Error: Instruction{self.data[instruction]} failed due to => trying to redefine already defined label\n", file=sys.stderr)
                    exit(52)  # trying to redefine already defined label
                self.labels[self.data[instruction]["LABEL"]["arg0"][key_value]] = instruction

    @staticmethod
    def convert_escape_sequence(string):
        esc = re.findall(r"(\\[0-9]{3})", string)
        for escape_seq in esc:
            string = string.replace(escape_seq, chr(int(escape_seq.lstrip('\\'))))
        return string
