#Standard Type and Variable Reverse Lib

##1. Overview

The ultimate goal of this project is to build a standard function lib that can conduct type and variable reverse analyses. We use existing analyses method--based on the CMU TIE--but encapsulate the analysis procedure and design a set of user friendly APIs, enable user to get the type and variable information from binary code and can integrate it easily into his own programs or existing tools.


##2. Code Structure

Under the 'project' folder is the source code of this project. 


        project/   -----    source/
                   -----    include/
                   -----    xed-ia32/
                   -----    makefile


The 'source/' contains all the .cpp files, the 'include/' contains all the .h files. The folder 'xed-ia32/' contains the extra lib we use inorder to conduct our analysis. The lib, named 'x86 XED', helps to decode the instruction and get the operand.


##3. Usage

1. Get the code.

    Directly clone from the github. URL: https://github.com/honeynet/binary_reverse_lib.

2. Compile 

    After get the source, cd into the 'project/' folder and type 'make' to compile the source. The extra lib is inside the folder, and it doesn't need any other lib.

    The compiler used in the makefile is **G++**. So make sure you contain the compiler. (Don't need to be the latest version.)


3. Run

    After compile, there would be a executable file called "REVERSE", directly run the executable, and you can see the results.


#####Notice:

* The code it self is flexiable, which means you can compile it whether under x86\_32 or x86\_64. But the analysis target is **merely x86\_32**, make sure that you have provided the correct binary.  

* The project currently is not a lib, it's a complete program. file 'project/source/main.cpp' contains the main function of this project.

    There are two main function in this project:
           
         line 48:   bool VariableRecovery(void *startaddress, void *endAddr, std::map<int, AbstractVariable*> &container)

    which is responsible for extracting variables in the binary code, and 

         line 100:  bool TypeInference(void *startaddress, void *endAddr, std::map<int, AbstractVariable*> &container)

    Which will inference the type of those variables.

    We put the binary code in a char array 
        
        line 170:   unsigned char code[] = "\x55\x89\xe5\x83\xe4\xf0\x83\xec\x30..."

    and call the two functions in the _main_ function, providing the begin and end address of the arrary. So if you want to change the testing code, just change the char array. 


* The folder 'tools' under the root directory contains the source of the testing code used in this project, together with a shell script 'x.sh'. 

    You can compile the code using '-c' flag to get the object file, and then  use the shell script to get the '\x55\x89...' form, which you can directly put into the source code of the project as the char arrary.

