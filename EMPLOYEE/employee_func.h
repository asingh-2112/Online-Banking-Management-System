#ifndef EMPLOEE_DETAILS
#define EMPLOEE_DETAILS
#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "./employee_details.h"
#include "../CUSTOMER/customer_functions.h"
#include "../TRANSACTIONS/transaction_func.h"
#include "../FEEDBACK/feedback_func.h"
#include "../FEEDBACK/feedback_struct.h"
#include "../COMMON/common.h"

#define EMPLOYEE_FILE "./EMPLOYEE/employee.txt"


int employee_login(int connFD,bool isManager);
int employee_operations(int connFD,bool isManager);
int add_employee(int connFD);
bool modify_employee(int connFD);
bool change_emp_password(int connFD);
int add_feedback(int connFD,int id);
int view_feedback(int connFD);

char empname[20];
int empId;
int sem_id;
struct sembuf sem_operation;

/*<-------------------------------------------------------------------------------------->*/
//                      EMPLOYEE LOGIN
/*<-------------------------------------------------------------------------------------->*/

int employee_login(int connFD,bool isManager){
    char readBuffer[500];
    char writeBuffer[500];
    int wr,rd,fd,attempts=3;
    memset(readBuffer,0,sizeof(readBuffer));
    memset(writeBuffer,0,sizeof(writeBuffer));
    while(1){
        if(isManager){
            strcat(writeBuffer,"Hello Manager! Enter your credentials:\nEnter you Manager Id");
        }
        else{
            strcat(writeBuffer,"Hello Employee! Enter your credentials:\nEnter you Employee Id");
        }
        wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        if(wr==-1){
            perror("Error while asking the id from employee");
            return false;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        int id=atoi(readBuffer);
        memset(readBuffer,0,sizeof(readBuffer));
        if(rd==-1){
            perror("Error while taking the id from employee");
            return false;
        }
        wr=write(connFD,"Enter your password",strlen("Enter your password"));
        if(wr==-1){
            perror("Error while asking the passowrd from employee");
            return false;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
            perror("Error while taking the passowrd from employee");
            return false;
        }

        struct employeeDetails emp;
        fd=open(EMPLOYEE_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return false;
        }
        //lock
        struct flock flk;
                flk.l_type=F_RDLCK;
                flk.l_len=sizeof(struct employeeDetails);
                flk.l_pid=getpid();
                flk.l_start=0;
                flk.l_whence=(id-1)*sizeof(emp);

                fcntl(fd,F_SETLKW,&flk);
                rd=read(fd,&emp,sizeof(emp));
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
        close(fd);
    
        if(emp.id!=id){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            attempts-=1;
            if(attempts)
                if(isManager){

                    sprintf(writeBuffer,"Manager ID doesn't exits. Attempts left- %d\n",attempts);
                }
                else{
                    sprintf(writeBuffer,"Employee ID doesn't exits. Attempts left- %d\n",attempts);

                }
            else return 1;
        }
        else if(strcmp(emp.password,crypt(readBuffer,salt))!=0){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            attempts-=1;
            if(attempts)
                sprintf(writeBuffer,"Entered wrong Password. Attempts left- %d\n",attempts);
            else return 2;
        }
        else if(emp.isManager!=isManager){
            return 3;
        }
        else{
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            strcpy(empname,emp.name);
            empId=emp.id;
            return 0;
        }
    }

}


/*<------------------------------------------------------------------------------------->*/
//                      EMPLOYEE OPERATIONS
/*<------------------------------------------------------------------------------------->*/

int employee_operations(int connFD,bool isManager){
    char readBuffer[500];
    char writeBuffer[500];
    char temp[100];
    int wr,rd,fd,attempts=3;
    int r=employee_login(connFD,isManager);

    /*<-------------MANAGER------------->*/

    if(isManager){

        if(r==0){
                sem_id = init_semphore(empId, EMPLOYEE_FILE);
                lock(&sem_operation, sem_id);
                memset(writeBuffer,0,sizeof(writeBuffer));
            while(1){
                memset(readBuffer,0,sizeof(readBuffer));
                char tempBuffer[50];
                sprintf(tempBuffer,"\nWelcome %s\n",empname);
                strcat(writeBuffer,tempBuffer);
                strcat(writeBuffer,"Select from options to perform:\n\t1.Activate/Deactive account\n\t2.Assign Loan Application to Employees\n\t3.Review Customer FeedBack\n\t4.Change Password\n\t5.Log out\n\t6.Exit\n");
                wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                if(wr==-1){
                    perror("Error while asking the options from employee");
                    return false;
                }
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                if(rd==-1){
                    perror("Error while taking the options from employee");
                    return false;
                }
                int choice=atoi(readBuffer);
                switch (choice)
                {
                case 1: //Activate/Decative Account
                    /* code */
                    if(activeAccount(connFD)){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Customer Status Changed Successfully\n");
                    }
                    else{
                        strcpy(writeBuffer,"Customer Status not Changed\n");
                    }
                    break;

                case 2: //Assign Loan Aoolication
                    if(assignLoan(connFD)==1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Loan Assigned\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error while Assigning the loan\n");
                    }

                case 3: //Review Customer Feedback
                    int res=view_feedback(connFD);
                    if(res==1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Action successfull\n");
                    }
                    else if(res==2){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Nothing to show\n");
                    }
                    else if(res==-1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"NError while showing feedback\n");
                    }
                    break;
                case 4: //Change Password
                    if(change_emp_password(connFD)){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Password Changed Successfully\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error while changing the password\n");
                    }
                    break;
                case 5:
                    unlock(&sem_operation, sem_id);
                    return 0;
                    break;
                default:
                    unlock(&sem_operation, sem_id);
                    break;
                }
                if(choice==5){
                    break;
                }
            }
            return 0;
        }
        else if(r==1){
            return 1;
        }
        else if(r==2){
            return 2;
        }
        else if(r==3){
            return 3;
        }
    }
    
    /*<-------------EMPLOYEE------------->*/
    else{
        if(r==0){
            sem_id = init_semphore(empId, EMPLOYEE_FILE);
                lock(&sem_operation, sem_id);
                memset(writeBuffer,0,sizeof(writeBuffer));
            while(1){
                memset(readBuffer,0,sizeof(readBuffer));
                char tempBuffer[50];
                sprintf(tempBuffer,"\nWelcome %s\n",empname);
                strcat(writeBuffer,tempBuffer);
                strcat(writeBuffer,"Select from options to perform:\n\t1.Add new Customer\n\t2.Modify Customer Details\n\t3.Approve/Reject:Loan\n\t4.View Pending Loans\n\t5.View your loans on which Action has been Taken\n\t6.View Customer Transaction\n\t7.Change Password\n\t8.Logout\n\t9.Exit\n");
                wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                if(wr==-1){
                    perror("Error while asking the options from employee");
                    return false;
                }
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                if(rd==-1){
                    perror("Error while taking the options from employee");
                    return false;
                }
                int choice=atoi(readBuffer);
                switch (choice)
                {
                case 1: //Add Customer
                    /* code */
                    int id=add_customer(connFD);
                    if(id!=0){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        sprintf(writeBuffer,"Customer added successfully with Account Number: %d\n",id);
                        // strcpy(writeBuffer,"Customer added successfully\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error while adding Customer\n");
                    }
                    break;

                case 2: //Modify Customer Details
                    if(modify_customer(connFD)){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Customer details modified successfully\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error while modifying details of Customer\n");
                    }
                    break;

                case 3: //Approve or Reject Loan
                    int res=accept_reject_loan(connFD,empId);
                    if(res==1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Action Taken Successfully\n");
                    }
                    else if(res==2){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error while modifying details of Customer\n");
                    }
                    break;
                case 4: //View Assigned Loan
                    res=view_assigned_loans(connFD,empId,true);
                    if(res==1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Viewed Successfully\n");
                    }
                    else if(res==3){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Nothing to Show\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error in wiewing\n");
                    }
                    break;
                case 5:
                    res=view_assigned_loans(connFD,empId,false);
                    if(res==1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Viewed Successfully\n");
                    }
                    else if(res==3){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Nothing to Show\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error in wiewing\n");
                    }
                    break;
                case 6: //View Customer Transaction
                    res=view_transaction(connFD,-1,false);
                    if(res==-1){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Error while viewing\n");
                    }
                    if(res==2){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"No Transaction\n");
                    }
                    break;
                case 7: //Change Password
                    if(change_emp_password(connFD)){
                        memset(writeBuffer,0,sizeof(writeBuffer));
                        memset(readBuffer,0,sizeof(readBuffer));
                        strcpy(writeBuffer,"Password Changed Successfully\n");
                    }
                    else{
                        strcpy(writeBuffer,"Error while changing the password\n");
                    }
                    break;
                case 8:
                    unlock(&sem_operation, sem_id);
                    return 0;
                    break;
                default:
                    unlock(&sem_operation, sem_id);
                    break;
                }
                if(choice==8){
                    break;
                }
            }
            return 0;
        }
        else if(r==1){
            return 1;
        }
        else if(r==2){
            return 2;
        }
        else if(r==3){
            return 3;
        }
    }
    return 0;
}


/*<------------------------------------------------------------------------------------->*/
//                      ADD NEW EMPLOYEE
/*<------------------------------------------------------------------------------------->*/


int add_employee(int connFD){
    char writeBuffer[1000];
    char readBuffer[1000];
    bool allok=false;
    struct employeeDetails emp,prevEmp;
    int fd=open(EMPLOYEE_FILE,O_RDONLY,0777);
    if(fd==-1 && errno==ENOENT){
        emp.id=1;
    }
    else if(fd==-1){
        perror("Error in opening the file_1");
        return 0;
    }
    else {
        int offset=lseek(fd, -sizeof(struct employeeDetails), SEEK_END);
        if (offset== -1) {
                    perror("Error seeking in file__2");
                    // close(fd);
                    return 0;
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
    strcat(writeBuffer,"\nEnter Employee Name");
    int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    if(wr==-1){
        perror("Error in taking name as input\n");
        return false;
    }
    memset(readBuffer,0,sizeof(readBuffer));
    int rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading name as input\n");
        return 0;
    }
    memset(emp.name,0,sizeof(emp.name));
    strcpy(emp.name,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //UserId
    wr=write(connFD,"\nEnter employee's userName",sizeof("Enter employee's userName"));
    if(wr==-1){
        perror("Error in taking userId as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading userId as input\n");
        return 0;
    }
    memset(emp.userId,0,sizeof(emp.userId));
    strcpy(emp.userId,readBuffer);

    //Age
    wr=write(connFD,"\nEnter employee's age",sizeof("Enter employee's age"));
    if(wr==-1){
        perror("Error in taking age as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading age as input\n");
        return 0;
    }
    emp.age=atoi(readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Gender
    wr=write(connFD,"\nEnter employee's gender",sizeof("Enter employee's gender"));
    if(wr==-1){
        perror("Error in taking gender as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading gender as input\n");
        return 0;
    }
    strcpy(emp.gender,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Role
    wr=write(connFD,"\nIs this Employee a Manager?.\nEnter 0 for NO.\nEnter 1 for YES",sizeof("Is this Employee a Manager?.\nEnter 0 for NO.\nEnter 1 for YES"));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(atoi(readBuffer)==1){
        memset(emp.role,0,sizeof(emp.role));
        strcpy(emp.role,"Manager");
        emp.isManager=true;
    }
    else{
        wr=write(connFD,"\nEnter employee's Role",sizeof("Enter employee's Role"));
        if(wr==-1){
            perror("Error in taking Role as input\n");
            return 0;
        }
        memset(readBuffer,0,sizeof(readBuffer));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
            perror("Error in reading Role as input\n");
            return 0;
        }
        memset(emp.role,0,sizeof(emp.role));
        strcpy(emp.role,readBuffer);
        emp.isManager=false;
    }
    memset(readBuffer,0,sizeof(readBuffer));

    //Password
    wr=write(connFD,"\nEnter employee's Password",sizeof("Enter employee's Password"));
    if(wr==-1){
        perror("Error in taking Password as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading Password as input\n");
        return 0;
    }
    memset(emp.password,0,sizeof(emp.password));
    strcpy(emp.password,crypt(readBuffer,salt));

    emp.count=0;
    for(int i=0;i<100;i++){
        emp.assignedLoans[i]=-1;
    }

    int newFd=open(EMPLOYEE_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
    if(newFd==-1){
        perror("Error while opening the employee file\n");
        return 0;
    }
    wr=write(newFd,&emp,sizeof(emp));
    close(newFd);

    return emp.id;
}


/*<------------------------------------------------------------------------------------->*/
//                  EMPLOYEE DETAILS MODIFICATION
/*<------------------------------------------------------------------------------------->*/

bool modify_employee(int connFD){
    char writeBuffer[500];
    char readBuffer[500];
    while(1){
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        int wr,rd,fd;

        wr=write(connFD,"Enter Id of Employee whose details is to be modified\n Or Enter exit to exit",sizeof("Enter Id of Employee whose details is to be modified\n Or Enter exit to exit"));
        if(wr==-1){
            perror("Error while asking id for modification\n");
            return false;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
            perror("Error while taking id for modification\n");
            return false;
        }
        if(strcmp(readBuffer,"exit")==0){
            return true;
        }
        int id=atoi(readBuffer);
        memset(readBuffer,0,sizeof(readBuffer));
        struct employeeDetails emp;
        fd=open(EMPLOYEE_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
        if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
                }
        struct flock flk;
                    flk.l_type=F_RDLCK;
                    flk.l_len=sizeof(struct employeeDetails);
                    flk.l_pid=getpid();
                    flk.l_start=0;
                    flk.l_whence=(id-1)*sizeof(emp);
                fcntl(fd,F_SETLKW,&flk);
                
                rd=read(fd,&emp,sizeof(emp));
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
        close(fd);
        // sprintf(writeBuffer,"Select which detail to be modified of the Employee\n id->%d\n",emp.id);
        // wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        if(id==(emp.id)){
            rechoose:
            sprintf(writeBuffer,"Employee whose details are to be modified\nId-> %d\nUserId-> %s\nName-> %s\nAge-> %d\n Gender-> %s\nRole-> %s\n",emp.id,emp.userId,emp.name,emp.age,emp.gender,emp.role);
            strcat(writeBuffer,"Select which detail to be modified of the Employee\n");
            strcat(writeBuffer,"\t1.Name\n\t2.Age\n\t3.Role\n\t4.userId\n\t5.ReChoose Id of the Employee\n\t6.Exit\n");
            wr=write(connFD,writeBuffer,sizeof(writeBuffer));

            if(wr==-1){
                perror("Error while asking options for modification\n");
                return false;
            }

            rd=read(connFD,readBuffer,sizeof(readBuffer));
            if(rd==-1){
                perror("Error while taking options for modification\n");
                return false;
            }
            int choice=atoi(readBuffer);
            memset(readBuffer,0,sizeof(readBuffer));
            memset(writeBuffer,0,sizeof(writeBuffer));
            switch (choice)
            {
            case 1: //Name
                /* code */
                wr=write(connFD,"Enter new Name",sizeof("Enter new Name"));
                if(wr==-1){
                    perror("Error while asking name for modification\n");
                    return false;
                }
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                if(rd==-1){
                    perror("Error while taking name for modification\n");
                    return false;
                }
                strcpy(emp.name,readBuffer);
                fd=open(EMPLOYEE_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the employee file\n");
                    return false;
                }
                offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
                }
                struct flock flk;
                    flk.l_type=F_WRLCK;
                    flk.l_len=sizeof(emp);
                    flk.l_pid=getpid();
                    flk.l_start=0;
                    flk.l_whence=(id-1)*sizeof(emp);
                    fcntl(fd,F_SETLKW,&flk);
                write(fd,&emp,sizeof(emp));
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
                close(fd);
                strcpy(writeBuffer,"Name changed Successfully\n");
                break;

            case 2: //Age
                /* code */
                wr=write(connFD,"Enter new Age",sizeof("Enter new Age"));
                if(wr==-1){
                    perror("Error while asking age for modification\n");
                    return false;
                }
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                if(rd==-1){
                    perror("Error while taking age for modification\n");
                    return false;
                }
                // strcpy(emp.name,readBuffer);
                emp.age=atoi(readBuffer);
                fd=open(EMPLOYEE_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the employee file\n");
                    return false;
                }
                offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
                }
                struct flock lock;
                    lock.l_type=F_WRLCK;
                    lock.l_len=sizeof(emp);
                    lock.l_pid=getpid();
                    lock.l_start=0;
                    lock.l_whence=(id-1)*sizeof(emp);
                    fcntl(fd,F_SETLKW,&lock);
                write(fd,&emp,sizeof(emp));
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
                close(fd);
                strcpy(writeBuffer,"Age changed Successfully\n");
                break;

            case 3: //Role
                /* code */
                if(!emp.isManager ){
                    wr=write(connFD,"Enter 1 to make this Employee a Manager.\nElse enter 0",sizeof("Enter 1 to make this Employee a Manager.\nElse enter 0"));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                if(emp.isManager || atoi(readBuffer)==0){
                    wr=write(connFD,"Enter new Role",sizeof("Enter new Role"));
                    if(wr==-1){
                        perror("Error while asking role for modification\n");
                        return false;
                    }
                    memset(readBuffer,0,sizeof(readBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                    if(rd==-1){
                        perror("Error while taking role for modification\n");
                        return false;
                    }
                    strcpy(emp.role,readBuffer);
                    emp.isManager=false;
                }
                else{
                    strcpy(emp.role,"Manager");
                    emp.isManager=true;
                }
                fd=open(EMPLOYEE_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the employee file\n");
                    return false;
                }
                offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
                }
                struct flock lck;
                    lck.l_type=F_WRLCK;
                    lck.l_len=sizeof(emp);
                    lck.l_pid=getpid();
                    lck.l_start=0;
                    lck.l_whence=(id-1)*sizeof(emp);
                    fcntl(fd,F_SETLKW,&lck);
                write(fd,&emp,sizeof(emp));
                lck.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lck);
                close(fd);
                strcpy(writeBuffer,"Role changed Successfully\n");
                break;
            case 4:
                wr=write(connFD,"Enter new UserName",sizeof("Enter new UserName"));
                if(wr==-1){
                    perror("Error while asking name for modification\n");
                    return false;
                }
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                if(rd==-1){
                    perror("Error while taking name for modification\n");
                    return false;
                }
                strcpy(emp.userId,readBuffer);
                fd=open(EMPLOYEE_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the employee file\n");
                    return false;
                }
                offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
                }
                struct flock flk_1;
                    flk_1.l_type=F_WRLCK;
                    flk_1.l_len=sizeof(emp);
                    flk_1.l_pid=getpid();
                    flk_1.l_start=0;
                    flk_1.l_whence=(id-1)*sizeof(emp);
                    fcntl(fd,F_SETLKW,&flk_1);
                write(fd,&emp,sizeof(emp));
                flk_1.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk_1);
                close(fd);
                strcpy(writeBuffer,"UserName changed Successfully\n");
                break;
            case 5: //exit
                /* code */
                break;
            case 6: //exit
                /* code */
                return true;
                break;
            default:
                strcpy(writeBuffer,"Wrong option Entered.\n");
                goto rechoose;
            }
        }
        else{
            wr=write(connFD,"Wrong Employee ID entered",sizeof("Wrong Employee ID entered"));
            if(wr==-1){
                perror("Error in displaying Wrong Employee ID entered\n");
                return false;
            }
        }
    }
}


/*<------------------------------------------------------------------------------------->*/
//                  EMPLOYEE CHANGE PASSWORD
/*<------------------------------------------------------------------------------------->*/

bool change_emp_password(int connFD){
    char writeBuffer[500];
    char readBuffer[500];
    while(1){
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        int wr,rd,fd;
        int id=empId;
        struct employeeDetails emp;
        fd=open(EMPLOYEE_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
        if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return false;
                }
        struct flock flk;
                    flk.l_type=F_RDLCK;
                    flk.l_len=sizeof(struct employeeDetails);
                    flk.l_pid=getpid();
                    flk.l_start=0;
                    flk.l_whence=(id-1)*sizeof(emp);
                fcntl(fd,F_SETLKW,&flk);
                
                rd=read(fd,&emp,sizeof(emp));
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
        close(fd);
                 //password
                /* code */
                wrong:
                strcat(writeBuffer,"Enter Your Current Password");
                wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                if(wr==-1){
                    perror("Error in writing Enter prev password");
                    return false;
                }
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                if(rd==-1){
                    perror("Error in reading the prev password");
                    return false;
                }
                if(strcmp(emp.password,crypt(readBuffer,salt))!=0){
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
                        strcpy(emp.password,crypt(readBuffer,salt));
                        fd=open(EMPLOYEE_FILE,O_RDWR,0777);
                        if(fd==-1){
                            perror("Error while opening the employee file\n");
                            return false;
                        }
                        offset=lseek(fd,(id-1)*sizeof(struct employeeDetails),SEEK_SET);
                        if (offset == -1) {
                            perror("Error seeking in file");
                            close(fd);
                            return -1;
                        }
                        struct flock flk;
                            flk.l_type=F_WRLCK;
                            flk.l_len=sizeof(emp);
                            flk.l_pid=getpid();
                            flk.l_start=0;
                            flk.l_whence=(id-1)*sizeof(emp);
                            fcntl(fd,F_SETLKW,&flk);
                        write(fd,&emp,sizeof(emp));
                        flk.l_type=F_UNLCK;
                        fcntl(fd,F_SETLKW,&flk);
                        close(fd);
                        return true;
                        // strcpy(writeBuffer,"Name changed Successfully\n");
                    }
                }
            return false;
        
    }
}


#endif