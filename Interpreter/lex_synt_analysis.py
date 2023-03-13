import re
import xml.etree.ElementTree as ET

instructions = {
    "CREATEFRAME": {"arg_count": 0},
    "PUSHFRAME": {"arg_count": 0},
    "POPFRAME": {"arg_count": 0},
    "RETURN": {"arg_count": 0},
    "BREAK": {"arg_count": 0},
    "DEFVAR": {"arg_count": 1, "arg1": "var"},
    "CALL": {"arg_count": 1, "arg1": "label"},
    "PUSHS": {"arg_count": 1, "arg1": "symb"},
    "POPS": {"arg_count": 1, "arg1": "var"},
    "WRITE": {"arg_count": 1, "arg1": "symb"},
    "LABEL": {"arg_count": 1, "arg1": "label"},
    "JUMP": {"arg_count": 1, "arg1": "label"},
    "EXIT": {"arg_count": 1, "arg1": "symb"},
    "DPRINT": {"arg_count": 1, "arg1": "symb"},
    "MOVE": {"arg_count": 2, "arg1": "var", "arg2": "symb"},
    "INT2CHAR": {"arg_count": 2, "arg1": "var", "arg2": "symb"},
    "STRLEN": {"arg_count": 2, "arg1": "var", "arg2": "symb"},
    "READ": {"arg_count": 2, "arg1": "var", "arg2": "type"},
    "TYPE": {"arg_count": 2, "arg1": "var", "arg2": "symb"},
    "NOT": {"arg_count": 2, "arg1": "var", "arg2": "symb"},
    "ADD": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "SUB": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "MUL": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "IDIV": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "AND": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "OR": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "LT": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "GT": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "EQ": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "STRI2INT": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "GETCHAR": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "SETCHAR": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "CONCAT": {"arg_count": 3, "arg1": "var", "arg2": "symb", "arg3": "symb"},
    "JUMPIFEQ": {"arg_count": 3, "arg1": "label", "arg2": "symb", "arg3": "symb"},
    "JUMPIFNEQ": {"arg_count": 3, "arg1": "label", "arg2": "symb", "arg3": "symb"}
}

"""
This is a class for lexical and syntactical checker of an xml notation
and IPPcode20 whose instructions are contained in xml format.
Object should only call the starter function 'xml_parser' and the analysis
will start.
If any error occurs, the program will exit with error code 32.
"""


class LexSyntAnalysis:
    def __init__(self, source, file_type):
        self.source = source
        self.type = file_type

    def xml_parser(self):
        if self.type is "file":  # it's file
            try:
                file = open(self.source, 'r')
                self.check_first_line(file, self.type)
                file.close()
            except FileNotFoundError:
                exit(11)
            try:
                tree = ET.parse(self.source)
                root = tree.getroot()
            except ET.ParseError:
                exit(31)

        else:  # it's string
            self.check_first_line(self.source, self.type)
            try:
                root = ET.fromstring(self.source)
            except ET.ParseError:
                exit(31)

        self.parse_root(root)
        self.parse_root_subelements(root)

    @staticmethod
    def parse_root(root):
        if root.attrib.get("language") is not None:
            if root.attrib["language"] != "IPPcode20":
                exit(32)  # wrong language
        else:
            exit(32)  # language element not found

        for attr in root.attrib:
            if attr != "language" and attr != "description" and attr != "name":
                exit(32)  # invalid element

    def parse_root_subelements(self, root):  # finding the keyword 'instruction'
        order_cnt = 1
        for child in root:
            if child.tag != "instruction":
                exit(32)  # invalid element
            for key in child.attrib:
                if key == "order":
                    if int(child.attrib[key]) != order_cnt:
                        exit(32)  # invalid order count
                    order_cnt += 1
                if key == "opcode":
                    opcode_label = child.attrib[key]
                    if opcode_label.upper() not in instructions:  # conversion to upper characters (case insensitive)
                        exit(32)  # unknown opcode
                if key != "order" and key != "opcode":
                    exit(32)  # invalid element attribute (only opcode/order are valid)

            self.parse_subelement_subelements(child, opcode_label.upper())

    def parse_subelement_subelements(self, subelement, opcode_label):
        symbol = ["int", "string", "bool", "nil"]
        variable = "var"
        label = "label"
        type = "type"

        arg_counter = 0
        for sub_child in subelement:
            arg_counter += 1
            for attr in sub_child.attrib:  # checking all elements in attribute arg
                if attr != "type":
                    exit(32)  # invalid element
            if instructions[opcode_label].get("arg_count") == 0:  # if the loop starts then it's an error because it should not start
                exit(32)
            if arg_counter == 1:
                if(sub_child.tag != "arg1" or sub_child.attrib.get("type") is None) or \
                  (instructions[opcode_label].get("arg1") == "var" and sub_child.attrib.get("type") != variable) or \
                  (instructions[opcode_label].get("arg1") == "symb" and (sub_child.attrib.get("type") not in symbol and sub_child.attrib.get("type") != variable)) or \
                  (instructions[opcode_label].get("arg1") == "label" and sub_child.attrib.get("type") != label) or \
                  (instructions[opcode_label].get("arg1") == "type" and sub_child.attrib.get("type") != type):
                    exit(32)
            if arg_counter == 2:
                if(sub_child.tag != "arg2" or sub_child.attrib.get("type") is None) or \
                  (instructions[opcode_label].get("arg2") == "var" and sub_child.attrib.get("type") != variable) or \
                  (instructions[opcode_label].get("arg2") == "symb" and (sub_child.attrib.get("type") not in symbol and sub_child.attrib.get("type") != variable)) or \
                  (instructions[opcode_label].get("arg2") == "label" and sub_child.attrib.get("type") != label) or \
                  (instructions[opcode_label].get("arg2") == "type" and sub_child.attrib.get("type") != type):
                    exit(32)
            if arg_counter == 3:
                if(sub_child.tag != "arg3" or sub_child.attrib.get("type") is None) or \
                  (instructions[opcode_label].get("arg3") == "var" and sub_child.attrib.get("type") != variable) or \
                  (instructions[opcode_label].get("arg3") == "symb" and (sub_child.attrib.get("type") not in symbol and sub_child.attrib.get("type") != variable)) or \
                  (instructions[opcode_label].get("arg3") == "label" and sub_child.attrib.get("type") != label) or \
                  (instructions[opcode_label].get("arg3") == "type" and sub_child.attrib.get("type") != type):
                    exit(32)

            if sub_child.attrib.get("type") in symbol:
                self.check_symbol(sub_child.text, sub_child.attrib.get("type"))
            if sub_child.attrib.get("type") == variable:
                self.check_variable(sub_child.text)
            if sub_child.attrib.get("type") == type:
                self.check_type(sub_child.text)
            if sub_child.attrib.get("type") == label:
                self.check_label(sub_child.text)

        if arg_counter > instructions[opcode_label].get("arg_count"):
            exit(32)  # wrong count of arguments

    @staticmethod
    def check_symbol(subelem_attrib, symbol_type):
        if symbol_type == "int":
            if re.match(r'^[+\-]?([0-9]|[1-9]+\d*)$', subelem_attrib) is None:
                print("tu1")
                exit(32)  # wrong int syntax
        if symbol_type == "string":
            if subelem_attrib is not None:
                if re.match(r'^((\\[0-9]{3})*|[^#\s\\])*$', subelem_attrib) is None:
                    print(subelem_attrib)
                    print("tu2")
                    exit(32)  # wrong string syntax
        if symbol_type == "bool":
            if re.match(r'(true|false)', subelem_attrib) is None:
                print("tu3")
                exit(32)  # wrong bool syntax
        if symbol_type == "nil":
            if re.match(r'^nil$', subelem_attrib) is None:
                print("tu4")
                exit(32)  # wrong nil

    @staticmethod
    def check_type(subelem_attrib):
        if re.match(r'(int|string|bool)', subelem_attrib) is None:
            print("tu5")
            exit(32)

    @staticmethod
    def check_variable(subelem_attrib):
        if re.match(r'^([GLT]F@(([a-zA-Z]+|[*_\-$&%?!]+)(\w|[_\-$&%*?!])*)+)+$', subelem_attrib) is None:
            print("tu6")
            exit(32)

    @staticmethod
    def check_label(subelem_attrib):
        if re.match(r'^(([a-zA-Z]+|[*_\-$&%?!]+)(\w|[_\-$&%*?!])*)+$', subelem_attrib) is None:
            print("tu8")
            exit(32)

    @staticmethod
    def check_first_line(input, type):
        if type == "file":
            for lines in input:
                if lines.strip() != "":
                    line = lines.rstrip()
                    break
        else:
            for lines in input.splitlines():
                if lines.strip() != "":
                    line = lines.rstrip()
                    break

        if line != "<?xml version=\"1.0\" encoding=\"UTF-8\"?>":
            exit(32)  # invalid first line of xml notation
