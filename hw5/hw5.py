import os
import sys
# all commands defined and stored as a string for printing
defined_commands = ["cd","exec","gcc","ls","man","more","mv","rm","cp","pwd","sh","touch","which","$path"]
defined_commands_string = ""
for command in defined_commands:
    defined_commands_string += command + " "
# intro
print("My cli 2020")
print("Legal commands: {}".format(defined_commands_string))
print("{} strings passed to argv[]".format(len(sys.argv)))
# putting all commands passed from command line into one string for parsing (except for 1st command which is the name of program)
commands = ""
for arg in sys.argv[1:len(sys.argv)]:
    commands += arg + " "
# traversing each command line argument by splitting up the commands by commas then checking if the first string in each is defined then executing
chunks = commands.split(",")
for chunk in chunks:
    chunk = chunk.strip(" ")
    print("next string is {}".format(chunk))
    cmd = chunk.split(" ")[0]
    if cmd in defined_commands:
        print("Command '{}' is one of predefined".format(cmd))
        os.system(chunk)
    else:
        print("Not one of legal commands:" + defined_commands_string)