import subprocess
import os
import shutil
import sys

# Colours for our output
class OutColours:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


# Log with pretty colours
def log(message, ok):
    if ok:
        print(OutColours.OKGREEN + "[  OK  ]\t" + message + OutColours.ENDC)
    else:
        print(OutColours.FAIL + "[ FAIL ]\t" + message + OutColours.ENDC)


# Parse expected value from source file
# Format #EXPECT:n
# This format is mainly for clarity when reading
def getexpectedoutput(path):
    exp = ""
    with open(path, "r") as f:
        top = f.readline()
        spl = top.split(':')
        if spl[0] == "#EXPECT":
            exp = spl[1].rstrip()
    # If no #EXPECT, return a blank string
    return exp


# Call program and see if the output value was what was expected
def testrun(path):
    process = subprocess.Popen(["./a.out"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()

    exp = getexpectedoutput(path)
    stdout = stdout.decode("utf-8").rstrip()
    if stdout.startswith(exp):
        log("Output '" + stdout + "' was expected", True)
    else:
        log("Expected '" + exp + "' but got '" + stdout + "'", False)
    print("")


# Call program and print stderr or stdout depending on success/failure
def testcompile(path):
    process = subprocess.Popen(["./simple", path, "-l"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()

    # See if compilation should fail
    exp = getexpectedoutput(path)

    # If the program is meant to fail, expect output on stderr
    if exp == "FAIL":
        if stderr:
            log("Compilation of '" + path +"' fails with error '" + stderr.decode("utf-8").rstrip() + "' as expected", True)
            print("")
    else:
        # Compilation isn't meant to fail
        if stderr:
            log("'" + path + "' did not compile: " + stderr.decode("utf-8"), False)
        else:
            log("'" + path + "' compiled successfully", True)
            testrun(path)


def main():
    try:
        # Copy file and make executable
        shutil.copyfile("../build/Compiler_exe/compiler_exe", "simple")
        subprocess.call(["chmod", "+x", "simple"])
        print(OutColours.HEADER + "Compiler binary found, testing:" + OutColours.ENDC)
    except:
        print(OutColours.FAIL + "No compiler binary found in '../build/Compiler_exe/compiler_exe'.\nPlease build before testing" + OutColours.ENDC)
        exit()


    testDir = "Test programs"
    for dirName, subDirs, files in os.walk(testDir):
        print(OutColours.HEADER + "Running tests in `" + dirName + "`" + OutColours.ENDC)
        for f in files:
            # Just test files with extension .simple
            if f.split('.')[-1] == "simple":
                path = os.path.join(dirName, f)
                testcompile(path)


if __name__ == "__main__":
    main()
