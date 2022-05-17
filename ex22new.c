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

void compileFile(char* pathToCFile, char* cFileWithOutSuffix, char* pathToStudentDirectory) {
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
        printf("inside pid 0\n");
		// need to print here perror "failed.."
        chdir(pathToStudentDirectory);
        // need to check if chdir worked
		if (execvp(command, commandArgs) < 0) {
			write(2,"Error in: execvp\n", 18);
            // need to move the error to errors.txt
            exit(-1);
		}
	} else {
		// waiting for child to terminate
		wait(NULL);
		return;
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
                        printf("and this is our c file without suffix: %s\n", cFileWithOutSuffix);
                        compileFile(pathToCFile, cFileWithOutSuffix, pathToStudentDirectory);
                    }
                }
            }
            printf("\n");
            pathToStudentDirectory[0] = '\0';
            closedir(dirOfSpecificStudent);
            }
    } 
    closedir(dirOfAllStudents);
}
