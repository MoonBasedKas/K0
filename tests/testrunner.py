#!/usr/bin/env python3
# TODO: Include the following result = subprocess.run([k0, target])

import sys
import subprocess
import os

colors = False

"""
Color information to improve readability.
"""
class color:
    RED = '\033[31m'
    BLACK = "\033[0;30m"
    RED = "\033[0;31m"
    GREEN = "\033[0;32m"
    BROWN = "\033[0;33m"
    BLUE = "\033[0;34m"
    PURPLE = "\033[0;35m"
    CYAN = "\033[0;36m"
    LIGHT_GRAY = "\033[0;37m"
    DARK_GRAY = "\033[1;30m"
    LIGHT_RED = "\033[1;31m"
    LIGHT_GREEN = "\033[1;32m"
    YELLOW = "\033[1;33m"
    LIGHT_BLUE = "\033[1;34m"
    LIGHT_PURPLE = "\033[1;35m"
    LIGHT_CYAN = "\033[1;36m"
    LIGHT_WHITE = "\033[1;37m"
    RESET = '\033[0m'

"""
Large data class for handling our test information.

folders:
    kotlin - kotlin errors
    error - expected errors
    k0 - success

prefixes:
    lex - lexical test
    syn - syntax test
    sem - semantic test
"""
class test:
    # Sets up test data.
    def __init__(self):

        self.targets = []
        self.executable = "../k0"
        self.subArgs = []
        
        self.success = 0
        self.lex = 0
        self.syntax = 0
        self.semantic = 0
        self.stdout = False

    """
    Parses a requested directory and generates our targets.
    """
    def parseDirectory(self, directory, files):
        for fp in files:
            self.addTarget(f"{directory}/{fp}", f"{directory}:None")
    
    def addTarget(self, file, expected):
        self.targets.append(data(file, expected))

    def setExecutable(self, string):
        self.executable = string

    """
    runs tests
    """
    def executeTests(self):
        for tar in self.targets:
            tar.execute(self.executable, self.subArgs)
        
        print("Test results")
        for tar in self.targets:
            print("-----")
            if tar.result == 0:
                printColor(color.GREEN, f"{tar.file} | SUCCESS with return code 0")
                self.success += 1

            elif tar.result == 1:
                printColor(color.RED, f"{tar.file} | LEXICAL ERROR with return code 1")
                self.lex += 1
                print(tar.resultData.stderr.decode("ASCII"))

            elif tar.result == 2:
                printColor(color.YELLOW, f"{tar.file} | SYNTAX ERROR with return code 2")
                print(tar.resultData.stderr.decode("ASCII"))
                self.syntax += 1

            elif tar.result == 3:
                printColor(color.LIGHT_PURPLE, f"{tar.file} | SEMATNIC ERROR with return code 3")
                self.semantic += 1
                print(tar.resultData.stderr.decode("ASCII"))
            else:
                printColor(color.LIGHT_BLUE, f"{tar.file} | UNKNOWN ERROR with return code {tar.result}")
                print(tar.resultData.stderr.decode("ASCII"))
            if self.stdout or self.subArgs != []:
                print(tar.resultData.stdout.decode("utf-8"))

    """
    Adds any interesting sub args to our executable
    """
    def loadOption(self, arg):
        self.subArgs.append(arg)

    def setStdout(self, stdout):
        self.stdout = stdout

"""
Holds information for a singular test.
"""
class data:
    def __init__(self, file, expected):
        self.file = file
        self.expectedResult = expected
        self.result = 0
        self.resultData = None

    """
    Executes a requested command with the provided file
    """
    def execute(self, executable, subargs):
        cmd = [executable, self.file]
        for arg in subargs:
            cmd.append(arg)

        result = subprocess.run(cmd, capture_output=True)
        
        self.resultData = result
        self.result = result.returncode
        
testData = test() # Bad coding practice but I need the POWER!

def main():
    global testData
    args = sys.argv
    args.pop(0) # Removes first elem
    if args == []: 
        defaultCase()
        return 0
    else:
        temp = None
        while args != []:
            temp = args.pop(0)
            if temp == "-color":
                global colors
                colors = True
            elif temp == "-symtab":
                testData.loadOption(temp)
            elif temp == "-tree":
                testData.loadOption(temp)
            elif temp == "-stdout":
                testData.setStdout(True)
        
            
        defaultCase()
        return 0


"""
Generic test case no fancy things.
"""
def defaultCase():
    global testData

    error = os.listdir("./errors")
    kotlin = os.listdir("./kotlin")
    k0 = os.listdir("./k0")

    if error != []: testData.parseDirectory("./errors", error)
    if kotlin != []: testData.parseDirectory("./kotlin", kotlin)
    if k0 != []: testData.parseDirectory("./k0", k0)

    testData.executeTests()

    return 0

"""
Tired of your code being written like its the 1960's? With this new invention 
any string can be turned into the RAINBOW*.

* just one specific color.
"""
def printColor(c, string):
    if colors == True:
        print(c, string, color.RESET)
    else:
        print(string)



if __name__ == "__main__":
    main()