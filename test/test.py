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

# Call program and print stderr or stdout depending on success/failure
def test(path):
    process = subprocess.Popen(["./simple", path, "-l"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr:
        log(path + " did not compile: " + stderr.decode("utf-8"), False)
    else:
        log(path + " compiled successfully", True)
    #TODO(James) Read expected value and compare to stdout?

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
                test(path)


if __name__ == "__main__":
    main()
