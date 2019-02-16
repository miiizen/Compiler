import subprocess
import os
import shutil

# Colours for our output
class outcolors:
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
        print(outcolors.OKGREEN + "[  OK  ]\t" + message + outcolors.ENDC)
    else:
        print(outcolors.FAIL + "[ FAIL ]\t" + message + outcolors.ENDC)

# Parse expected value from source file
# Format #EXPECT:n
# This format is mainly for clarity when reading
def getExpectedOutput(path):
    exp = ""
    with open(path, "r") as f:
        top = f.readline()
        spl = top.split(':')
        if(spl[0] == "#EXPECT"):
            exp = spl[1].rstrip()
    # If no #EXPECT, return a blank string
    return exp

# Call program and see if the output value was what was expected
def testRun(path):
    process = subprocess.Popen(["./a.out"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()

    exp = getExpectedOutput(path)
    stdout = stdout.decode("utf-8").rstrip()
    if(stdout.startswith(exp)):
        log("Output '" + stdout + "' was expected", True)
    else:
        log("Expected '" + exp + "' but got '" + stdout + "'", False)
    print("")

# Call program and print stderr or stdout depending on success/failure
def testCompile(path):
    process = subprocess.Popen(["./simple", path, "-l"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr:
        log("'" + path + "' did not compile: " + stderr.decode("utf-8"), False)
    else:
        log("'" + path + "' compiled successfully", True)
        testRun(path)



def main():
    try:
        # Copy file and make executable
        shutil.copyfile("../build/Compiler_exe/compiler_exe", "simple")
        subprocess.call(["chmod", "+x", "simple"])
        print(outcolors.HEADER + "Compiler binary found, testing:" + outcolors.ENDC)
    except:
        print(outcolors.FAIL + "No compiler binary found in `../build/Compiler_exe/compiler_exe`.\nPlease build before testing" + outcolors.ENDC)
        exit()


    testDir = "Test programs"
    for dirName, subDirs, files in os.walk(testDir):
        print(outcolors.HEADER + "Running tests in `" + dirName + "`" + outcolors.ENDC)
        for f in files:
            # Just test files with extension .simple
            if(f.split('.')[-1] == "simple"):
                path = os.path.join(dirName, f)
                testCompile(path)


if __name__ == "__main__":
    main()
