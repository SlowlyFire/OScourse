// Gal Giladi 312491616
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

int compileFile(char* pathToCFile, char* cFileWithOutSuffix, char* pathToStudentDirectory) {
    // here we compile the c file, with given path to the file
    // we should use fork and child proccess
    char* command = "gcc";
    char* commandArgs[5];
    commandArgs[0] = "gcc";
    commandArgs[1] = pathToCFile;
    commandArgs[2] = "-o";
    commandArgs[3] = cFileWithOutSuffix;
    commandArgs[4] = NULL;
    // Forking a child
	// fork returns pid of the son
	pid_t pid = fork();
	// -1 failed
	// need to print here perror - fork failed
	if (pid == -1) {
		write(2,"Error in: fork\n", 16);
        // need to move the error to errors.txt
        exit(-1);
	// if fork=0 im in son proccess
	} else if (pid == 0) {
        printf("inside compile pid 0\n");
		// need to print here perror "failed.."
        chdir(pathToStudentDirectory);
        // need to check if chdir worked
		if (execvp(command, commandArgs) < 0) {
			write(1,"Error in: execvp\n", 18);
            // need to move the error to errors.txt
            exit(-1);
		}
	} else {
		// waiting for child to terminate
		int status;
        waitpid(pid, &status, 0);
        int result = WEXITSTATUS(status);
        return result;
	}
}

void runFile(char* confLine2, char* cFileWithOutSuffix, int fdInputFile, int fdOutputFile) {
    // here we run the c file, with given path to the file
    // and with given path to a file that containts the input we wish to use.
    // this input is line 2 of conf.txt
    // we should use fork and child proccess
    char command[150] = {'\0'};
    strcat(command, "./");
    strcat(command, cFileWithOutSuffix);
    char* commandArgs[3];
    commandArgs[0] = command;
    commandArgs[1] = confLine2;
    commandArgs[2] = NULL;
    //printf("%s %s %s %s\n", command, commandArgs[0], commandArgs[1], commandArgs[2]);
    // Forking a child
	// fork returns pid of the son
	pid_t pid = fork();
	// -1 failed
	// need to print here perror - fork failed
	if (pid == -1) {
		write(2,"Error in: fork\n", 16);
        // need to move the error to errors.txt
        exit(-1);
	// if fork=0 im in son proccess
	} else if (pid == 0) {
        printf("inside pid 0 of runFile\n");
        // redirection
        dup2(fdInputFile, 0);
        dup2(fdOutputFile, 1);
        //printf("after dupping in runFile\n");
		// need to print here perror "failed.."
        // need to check if chdir worked
		if (execvp(command, commandArgs) < 0) {
			write(1,"Error in: execvp\n", 19);
            // need to move the error to errors.txt
            exit(-1);
		}
	} else {
		// waiting for child to terminate
		wait(NULL);
		return;
	}
}

int compareOutputs(char* confLine3, char* pathToStudentDirectory) {
    // here we compare between to output files and return what comp.out gives us,
    char studentOutput[150] = {'\0'};
    strcat(studentOutput, pathToStudentDirectory);
    strcat(studentOutput, "/outputFile.txt");
    printf("studentOutput is: %s\n", studentOutput);
    // we should use fork and child proccess
    // there is a chance I need to give here a full path
    char* command = "./comp.out";
    char* commandArgs[4];
    commandArgs[0] = command;
    // confLine3 is excpectedOutput
    commandArgs[1] = confLine3;
    // there is a chance I need to give here a full path
    commandArgs[2] = studentOutput;
    commandArgs[3] = NULL;
    //printf("%s %s %s %s %s\n", command, commandArgs[0], commandArgs[1], commandArgs[2], commandArgs[3]);
    // Forking a child
	// fork returns pid of the son
	pid_t pid = fork();
	// -1 failed
	// need to print here perror - fork failed
	if (pid == -1) {
		write(2,"Error in: fork\n", 16);
        // need to move the error to errors.txt
        exit(-1);
	// if fork=0 im in son proccess
	} else if (pid == 0) {
        printf("inside pid 0 of compare\n");
        // going to the directory where comp.out is located
        chdir(pathToStudentDirectory);
        chdir("..");
        chdir("..");
		// need to print here perror "failed.."
        // need to check if chdir worked
		if (execvp(command, commandArgs) < 0) {
			write(1,"Error in: execvp\n", 1);
            // need to move the error to errors.txt
            exit(-1);
        }
	} else {
        // waiting for child to terminate
        int status;
        waitpid(pid, &status, 0);
        int result = WEXITSTATUS(status);
        return result;
	}
}

int main(int argc, char* argv[])
{
    int fd;
    char* confPath = argv[1];
    fd = open(confPath, O_RDONLY);
    if (fd == -1) {
        write(2,"Error in: open\n", 16);
        // need to move the error to errors.txt
        exit(-1);
    }
    printf("fd of confPath is: %d\n", fd);

    char* confLine1;
    char* confLine2;
    char* confLine3;
    char buff[450]={'\0'};
    int resultRead;

    resultRead = read(fd, buff, 450);
    if (resultRead == -1) {
        write(2,"Error in: read\n", 16);
        // need to move the error to errors.txt
        close(fd);
        exit(-1);
    }

    printf("configuration text is:\n%s\n", buff);

    confLine1 = strtok(buff, "\n");
    confLine2 = strtok(NULL, "\n");
    confLine3 = strtok(NULL, "\n");
    printf("confLine1: %s\n", confLine1);
    printf("confLine2: %s\n", confLine2);
    printf("confLine3: %s\n", confLine3);

    int resultsFd;
    resultsFd = open("results.csv", O_RDWR | O_TRUNC | O_APPEND | O_CREAT, 0777);
    if (resultsFd == -1) {
        write(2,"Error in: open\n", 16);
        // need to move the error to errors.txt
        exit(-1);
    }
    printf("fd of results.csv is: %d\n", resultsFd);

    // now we should open the directory we got on confLine1
    DIR* dirOfAllStudents = opendir(confLine1);
    if (dirOfAllStudents == NULL) {
        write(2,"can't open dirOfAllStudents\n", 29);
        // need to move the error to errors.txt
    }
    printf("opened dirOfAllStudents\n");
    // then we should open one of the directories inside confLine1 path
    // line of conf.txt, and then look for a c file.
    struct dirent *studentXDir; 
    while ( (studentXDir = readdir(dirOfAllStudents)) != NULL ) {
        if(strcmp(studentXDir -> d_name, ".") != 0 &&
        strcmp(studentXDir -> d_name, "..") != 0) {
            // flagCFile is 0 when there is no c file in directory
            int flagCFile = 0;
            printf("%s\n", studentXDir -> d_name); 
            // now we create the path to the student directory
            char pathToStudentDirectory[150] = {'\0'};
            strcat(pathToStudentDirectory, confLine1);
            strcat(pathToStudentDirectory, "/");
            strcat(pathToStudentDirectory, studentXDir -> d_name);
            printf("pathToStudentDirectory is: %s\n", pathToStudentDirectory);
            DIR* dirOfSpecificStudent = opendir(pathToStudentDirectory);
            if (dirOfSpecificStudent == NULL) {
                write(2,"can't open dirOfSpecificStudent\n", 33);
                // need to move the error to errors.txt
                exit(-1);
            }
            printf("opened\n");
            struct dirent *insideStudentDirectory; 
            int lengthOfFileName;
            while ( (insideStudentDirectory = readdir(dirOfSpecificStudent)) != NULL ) {
                if(strcmp(insideStudentDirectory -> d_name, ".") != 0 &&
                strcmp(insideStudentDirectory -> d_name, "..") != 0) {
                    printf("this is: %s\n", insideStudentDirectory -> d_name);
                    lengthOfFileName =  strlen(insideStudentDirectory -> d_name);
                    if((insideStudentDirectory -> d_name[lengthOfFileName-2]) == '.' 
                    && (insideStudentDirectory -> d_name[lengthOfFileName-1]) == 'c') {
                        // if we found a ".c" file in Student Directory, flagCFile is 1
                        flagCFile = 1;
                        printf("this is our c file: %s\n", insideStudentDirectory -> d_name);
                        // now we build the c file a full path
                        char pathToCFile[150] = {'\0'};
                        strcat(pathToCFile, pathToStudentDirectory);
                        strcat(pathToCFile, "/");
                        strcat(pathToCFile, insideStudentDirectory -> d_name);
                        printf("this is our c file full path: %s\n", pathToCFile);
                        char cFileWithOutSuffix[150] = {'\0'};
                        strcat(cFileWithOutSuffix, insideStudentDirectory -> d_name);
                        cFileWithOutSuffix[lengthOfFileName-1] = '\0';
                        cFileWithOutSuffix[lengthOfFileName-2] = '\0';
                        strcat(cFileWithOutSuffix, ".out");
                        printf("and this is our c file with out suffix: %s\n", cFileWithOutSuffix);
                        int compileResult = compileFile(pathToCFile, cFileWithOutSuffix, pathToStudentDirectory);
                        printf("this is compileResult: %d\n", compileResult);
                        // compileResult will be 0 if managed to compile, and 1 if couldn't compile
                        if (compileResult == 1) {
                            write(resultsFd, studentXDir -> d_name, strlen(studentXDir -> d_name));
                            write(resultsFd, ",10,COMPILATION_ERROR\n", 22);
                            continue;
                        }
                        int fdInputFile;
                        fdInputFile = open(confLine2, O_RDONLY);
                        if (fdInputFile == -1) {
                            write(2,"Error in: open\n", 16);
                            // need to move the error to errors.txt
                            exit(-1);
                        }
                        printf("fd of inputFile (confLine2) is: %d\n", fdInputFile);   
                        chdir(pathToStudentDirectory);
                        int fdOutputFile;
                        fdOutputFile = open("outputFile.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                        if (fdOutputFile == -1) {
                            write(2,"Error in: open\n", 16);
                            // need to move the error to errors.txt
                            exit(-1);
                        }
                        printf("fd of new created OutputFile is: %d\n", fdOutputFile);
                        
                        runFile(confLine2, cFileWithOutSuffix, fdInputFile, fdOutputFile);
                        // close files in the end of the use of fd
                        close(fdInputFile);
                        close(fdOutputFile);
                        int compareNum;
                        compareNum = compareOutputs(confLine3, pathToStudentDirectory);
                        printf("compareNum is: %d\n", compareNum);

                        // append grades of students to results.csv file 
                        write(resultsFd, studentXDir -> d_name, strlen(studentXDir -> d_name));
                        // if files are different
                        if(compareNum == 2) {
                            write(resultsFd, ",50,WRONG\n", 10);
                        }
                        // if files are similar
                        if (compareNum == 3) {
                            write(resultsFd, ",75,SIMILAR\n", 12);
                        }
                        if(compareNum == 1) {
                            write(resultsFd, ",100,EXCELLENT\n", 15);
                        }
                    }
                }
            }
            if (flagCFile == 0) {
                write(resultsFd, studentXDir -> d_name, strlen(studentXDir -> d_name));
                write(resultsFd, ",0,NO_C_FILE\n", 13);
            }
            printf("\n");
            pathToStudentDirectory[0] = '\0';
            closedir(dirOfSpecificStudent);
        }
    } 
    write(resultsFd, "\n", 1);
    close(resultsFd);
    closedir(dirOfAllStudents);
    //return;
}
