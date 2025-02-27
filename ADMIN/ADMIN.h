#ifndef ADMIN_FUNCTION
#define ADMIN_FUNCTION
#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "./admin_detail.h"
#include "../EMPLOYEE/employee_func.h"
#include "../COMMON/common.h"
// #include <bcrypt.h>
#define ADMIN_ID "admin-1"

// #define ADMIN_PASSWORD "admin123"

bool admin_operations(int connFD);
bool login(int connFD);
bool change_password(int connFD);
int sem_id;
struct sembuf sem_operation;


/*<------------------------------------------------------------------------------------->*/
//                          ADMIN LOGIN
/*<------------------------------------------------------------------------------------->*/

bool login(int connFD){
    
    char readBuffer[1000];
    char writeBuffer[1000];
    bool userFound=false;
    memset(writeBuffer,0,sizeof(writeBuffer));
    while(1){
    memset(readBuffer,0,sizeof(readBuffer));
    strcat(writeBuffer,"\nWelcome Admin\nEnter your credentials\n\nEnter your login ID:");
    int wr=write(connFD,writeBuffer,strlen(writeBuffer));
    if(wr==-1){
        perror("Error in writing WELCOME & login ID to client");
        return false;
    }
    int rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading the login id");
        return false;
    }
    if(strcmp(ADMIN_ID,readBuffer)==0){
        userFound=true;
        break;
    }
    //password chaeck
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        strcpy(writeBuffer,"User ID not found. Try Again\n");
        // rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
            perror("Error in reading the login id");
            return false;
        }
        // if(strcmp(ADMIN_ID,readBuffer)==0){
        //     userFound=true;
        // }
    }
        struct adminDetails admin;
            memset(writeBuffer,0,sizeof(writeBuffer));
        while(1){

            memset(readBuffer,0,sizeof(readBuffer));

            strcat(writeBuffer,"\nEnter your password:");
            int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
            if(wr==-1){
                perror("Error in asking the password");
                return false;
            }
            int rd=read(connFD,readBuffer,sizeof(readBuffer));
            if(rd==-1){
                perror("Error in reading the password");
                return false;
            }
            else{
                char temp[1000];
                int fd=open("./ADMIN/admin.txt",O_RDONLY);
                if(fd==-1){
                    perror("Error in opening file");
                    return false;
                }
                struct flock flk;
                flk.l_type=F_RDLCK;
                flk.l_len=0;
                flk.l_pid=0;
                flk.l_start=0;
                flk.l_whence=0;
                fcntl(fd,F_SETLKW,&flk);

                fd=read(fd,&admin,sizeof(admin));
                if(fd==-1){
                    perror("error in reading");
                    return false;
                }
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
                strcpy(temp,crypt(readBuffer,salt));
                if(strcmp(temp,admin.password)!=0){
                    strcpy(writeBuffer,"Password not matched. Try Again\n");
                    // memset(readBuffer,0,sizeof(readBuffer));
                    // int rd=read(connFD,readBuffer,sizeof(readBuffer));
                    // return false;
                }
                else break;
            }
            // }
    }
        return true;
    

}


/*<------------------------------------------------------------------------------------->*/
//                          CHANGE PASSWORD
/*<------------------------------------------------------------------------------------->*/

bool change_password(int connFD){
    char readBuffer[100];
    char writeBuffer[100];
    memset(writeBuffer,0,sizeof(writeBuffer));
    memset(readBuffer,0,sizeof(readBuffer));
    wrong:
    strcat(writeBuffer,"Enter Your Current Password");
    int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    if(wr==-1){
            perror("Error in writing Enter prev password");
            return false;
        }
    int rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
            perror("Error in reading the prev password");
            return false;
    }
    struct adminDetails admin;
    int fd=open("./ADMIN/admin.txt",O_RDONLY);
    if(fd==-1){
        perror("Error in opening the file for reading password");
        return false;
    }
    struct flock flk;
                flk.l_type=F_RDLCK;
                flk.l_len=sizeof(struct adminDetails);
                flk.l_pid=getpid();
                flk.l_start=0;
                flk.l_whence=0;
            fcntl(fd,F_SETLKW,&flk);
            rd=read(fd,&admin,sizeof(admin));
            flk.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&flk);
            close(fd);
    if(rd==-1){
                perror("Error in reading the admin details in struct");
                return false;
        }
    close(fd);
    if(strcmp(admin.password,crypt(readBuffer,salt))!=0){
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        strcpy(writeBuffer,"Entered wrong current password.\n");
        goto wrong;
    }
    else{
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        char temp[100];
        not_match:
        strcat(writeBuffer,"Enter new Password");
        wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        if(wr==-1){
            perror("Error in writing the new password");
            return false;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
                perror("Error in reading the new password");
                return false;
        }
        wr=write(connFD,"ReEnter new Password",sizeof("ReEnter new Password"));
        if(wr==-1){
                perror("Error in writing the new password");
                return false;
        }
        rd=read(connFD,temp,sizeof(temp));
        if(strcmp(readBuffer,temp)!=0){
            memset(writeBuffer,0,sizeof(writeBuffer));
            strcpy(writeBuffer,"Password not matched\n");
            memset(readBuffer,0,sizeof(readBuffer));
            memset(temp,0,sizeof(temp));
            goto not_match;
        }
        else{
            memset(writeBuffer,0,sizeof(writeBuffer));
            // strcpy(writeBuffer,"Password matched");
            strcpy(admin.password,crypt(readBuffer,salt));
            int fd=open("./ADMIN/admin.txt",O_WRONLY);
            if(fd==-1){
                perror("Error in opening the file for writing password");
                return false;
            }
            struct flock flk;
                flk.l_type=F_WRLCK;
                flk.l_len=sizeof(struct adminDetails);
                flk.l_pid=getpid();
                flk.l_start=0;
                flk.l_whence=0;
            fcntl(fd,F_SETLKW,&flk);
            wr=write(fd,&admin,sizeof(admin));
            flk.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&flk);
            close(fd);
            if(wr==-1){
                perror("Error in writing the new password into the file");
                return false;
            }
            return true;
        }

    }

}


/*<------------------------------------------------------------------------------------->*/
//                      ADMIN OPERATIONS
/*<------------------------------------------------------------------------------------->*/

bool admin_operations(int connFD){
    if(login(connFD)){
        char t[30];
        strncpy(t,ADMIN_ID+6,1);

        sem_id = init_semphore(atoi(t), "./ADMIN/admin.txt");

        lock(&sem_operation, sem_id);
        char readBuffer[1000];
        char writeBuffer[1000];
        strcpy(writeBuffer,"Succesfully Logged in your account as Admin\n");
        strcat(writeBuffer,"Welcome Admin\n");

        while(1){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            strcat(writeBuffer,"\n\t1.Add New Bank Employee\n\t2.Modify Employee Details\n\t3.Modify Customer Details\n\t4.Change Password\n\t5.Logout\n\t6.Exit\n");
            strcat(writeBuffer,"Enter your choice");
            int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
            if(wr==-1){
                perror("Error in Giving the choice to admin");
            }
            int rd=read(connFD,readBuffer,sizeof(readBuffer));
            if(rd==-1){
                perror("Error in reading the choice for admin operations");
            }
            int choice=atoi(readBuffer);
            switch (choice)
            {
            case 1: //Add employee
                /* code */
                int id=add_employee(connFD);
                if(id!=0){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    sprintf(writeBuffer,"Added Employee Successfully with Employee Id: %d. Enter 1 to exit",id);
                    int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    int rd=read(connFD,readBuffer,sizeof(readBuffer));
                    memset(writeBuffer,0,sizeof(writeBuffer));
                }
                else{
                int wr=write(connFD,"Error in Adding Employee. Enter 1 to exit",sizeof("Error in Adding Employee. Enter 1 to exit"));
                }

                break;
            case 2: //Modify Employee details
                /* code */
                if(modify_employee(connFD)){
                    int wr=write(connFD,"Modified details pf Employee Successfully. Enter 1\n",sizeof("Modified details pf Employee Successfully. Enter 1\n"));
                    int rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else{
                int wr=write(connFD,"Error in Modifying details of Employee",sizeof("Error in Modifying details of Employee"));
                }
                break;
            case 3: //Modify Customer
                /* code */
                if(modify_customer(connFD)){
                    int wr=write(connFD,"Modified details pf customer Successfully. Enter 1\n",sizeof("Modified details pf customer Successfully. Enter 1\n"));
                    int rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else{
                int wr=write(connFD,"Error in Modifying details of Customer",sizeof("Error in Modifying details of Customer"));
                }
                break;
            case 4: //Change Password
                /* code */
                if(change_password(connFD)){
                    int wr=write(connFD,"Password of Admin Changed Succesfully\n",sizeof("Password of Admin Changed Succesfully\n"));
                    int rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else{
                int wr=write(connFD,"Error in Changing the Password",sizeof("Error in Changing the Password"));
                }
                break;
            case 5: //Logout
                /* code */
                unlock(&sem_operation, sem_id);
                return true;
                break;
            case 6: //Exit
                /* code */
                unlock(&sem_operation, sem_id);
                return true;
                break;

            default:
                unlock(&sem_operation, sem_id);
                break;
            }
        }
        
        // int wr=write(connFD,"Enter 1 to log out\n",sizeof("Enter 1 to log out\n"));
    }
    else{
        int wr=write(connFD,"Not able to login into your account\n",sizeof("Not able to login into your account\n"));
        return false;
    }
}
#endif


/*bool add_employee(int connFD){
    char writeBuffer[1000];
    char readBuffer[1000];
    struct employeeDetails emp,prevEmp;
    int fd=open(EMPLOYEE_FILE,O_RDONLY,0777);
    if(fd==-1 && errno==ENOENT){
        emp.id=0;
    }
    else if(fd==-1){
        perror("Error in opening the file_1");
    }
    else {
        int offset=lseek(fd, -sizeof(struct employeeDetails), SEEK_END);
        if (offset== -1) {
                    perror("Error seeking in file__2");
                    // close(fd);
                    return -1;
        }
        struct flock flk;
                    flk.l_type=F_RDLCK;
                    flk.l_len=sizeof(struct employeeDetails);
                    flk.l_pid=getpid();
                    flk.l_start=offset;
                    flk.l_whence=SEEK_SET;
                    fcntl(fd,F_SETLKW,&flk);
        read(fd,&prevEmp,sizeof(prevEmp));
        flk.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&flk);
        close(fd);
        emp.id=prevEmp.id+1;
    }
    strcpy(writeBuffer,"Enter Details of New Employee\n");

    //Employee Name
    strcat(writeBuffer,"Enter Employee Name");
    int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    if(wr==-1){
        perror("Error in taking name as input\n");
        return false;
    }
    memset(readBuffer,0,sizeof(readBuffer));
    int rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading name as input\n");
        return false;
    }
    memset(emp.name,0,sizeof(emp.name));
    strcpy(emp.name,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //UserId
    wr=write(connFD,"Enter employee's userId",sizeof("Enter employee's userId"));
    if(wr==-1){
        perror("Error in taking userId as input\n");
        return false;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading userId as input\n");
        return false;
    }
    strcpy(emp.userId,readBuffer);

    //Age
    wr=write(connFD,"Enter employee's age",sizeof("Enter employee's age"));
    if(wr==-1){
        perror("Error in taking age as input\n");
        return false;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading age as input\n");
        return false;
    }
    emp.age=atoi(readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Gender
    wr=write(connFD,"Enter employee's gender",sizeof("Enter employee's gender"));
    if(wr==-1){
        perror("Error in taking gender as input\n");
        return false;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading gender as input\n");
        return false;
    }
    strcpy(emp.gender,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Role
    wr=write(connFD,"Enter employee's Role",sizeof("Enter employee's Role"));
    if(wr==-1){
        perror("Error in taking Role as input\n");
        return false;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading Role as input\n");
        return false;
    }
    strcpy(emp.role,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Password
    wr=write(connFD,"Enter employee's Password",sizeof("Enter employee's Password"));
    if(wr==-1){
        perror("Error in taking Password as input\n");
        return false;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading Password as input\n");
        return false;
    }
    strcpy(emp.password,crypt(readBuffer,salt));
    int newFd=open(EMPLOYEE_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
    if(newFd==-1){
        perror("Error while opening the employee file\n");
        return false;
    }
    wr=write(newFd,&emp,sizeof(emp));
    close(newFd);

    return true;
}*/
