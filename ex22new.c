// Gal Giladi 312491616
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

void openDirectoryOfStudent(char* pathToStudentDirectory) {
    DIR* dirOfSpecificStudent = opendir(pathToStudentDirectory);
    if (dirOfSpecificStudent == NULL) {
        write(2,"can't open dirOfSpecificStudent\n", 33);
        // need to move the error to errors.txt
        exit(-1);
    }
    printf("opened\n");
    struct dirent *insideStudentDirectory; 
    while ( (insideStudentDirectory = readdir(dirOfSpecificStudent)) != NULL ) {
        if(strcmp(insideStudentDirectory -> d_name, ".") != 0 &&
        strcmp(insideStudentDirectory -> d_name, "..") != 0) {
            printf("this is: %s\n", insideStudentDirectory -> d_name); 
        }
    }
    printf("\n");
    pathToStudentDirectory[0] = '\0';
    closedir(dirOfSpecificStudent);
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
            openDirectoryOfStudent(pathToStudentDirectory);
        }
    } 
    closedir(dirOfAllStudents);
}
