#ifndef CUSTOMER_FUNCTIONS
#define CUSTOMER_FUCTIONS
#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "./loan_struct.h"
#define LOAN_FILE "./LOAN/loan.txt"
#define CUSTOMER_FILE "./CUSTOMER/customer.txt"
#define EMP_FILE "./EMPLOYEE/employee.txt"
#include "../CUSTOMER/customer_struct.h"
#include "../EMPLOYEE/employee_func.h"

int apply_loan(int connFD,bool isCust,int Id);
int haveLoan(int id);
bool update_loan(int account,char loanId[10]);
int assignLoan(int connFD);
int view_assigned_loans(int connFD,int id,bool pending);
int accept_reject_loan(int connFD,int id);


/*<------------------------------------------------------------------------------------->*/
//                  APPLY LOAN
/*<------------------------------------------------------------------------------------->*/

int apply_loan(int connFD,bool isCust,int Id){
    char writeBuffer[100];
    char readBuffer[100];
    int wr,rd,fd,custId;
    if(isCust){
        int res=haveLoan(Id);
        if(res==0){
            return 2;
        }
        else if(res==-1){
            return 0;
        }
        custId=Id;
        wr=write(connFD,"Enter the Amount for LOAN:",sizeof("Enter the Amount for LOAN:"));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
    }
    else{

    }
    struct loanDetails loan,prevLoan;
    loan.cust_acc_num=custId;
    loan.amount=atoi(readBuffer);
    loan.complete=false;
    loan.status=1;

    //Loan Details
    fd=open(LOAN_FILE,O_RDONLY,0777);
    if(fd==-1 && errno==ENOENT){
        strcpy(loan.loan_id,"LN0001");
    }
    else if(fd==-1){
        perror("Error in opening the file_1");
        return 0;
    }
    else {
        int offset=lseek(fd, -sizeof(struct loanDetails), SEEK_END);
        if (offset== -1) {
                    perror("Error seeking in file__2");
                    // close(fd);
                    return 0;
        }
        struct flock flk;
                    flk.l_type=F_RDLCK;
                    flk.l_len=sizeof(struct loanDetails);
                    flk.l_pid=getpid();
                    flk.l_start=offset;
                    flk.l_whence=SEEK_SET;
                    fcntl(fd,F_SETLKW,&flk);
        read(fd,&prevLoan,sizeof(prevLoan));
        flk.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&flk);
        close(fd);
        char temp[50];
        strncpy(temp,prevLoan.loan_id+2,4);
        strcpy(loan.loan_id,"LN");
        int t=atoi(temp)+1;
        memset(temp,0,sizeof(temp));
        sprintf(temp,"%04d",t);
        strcat(loan.loan_id,temp);

    }
        int newFd=open(LOAN_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
        if(newFd==-1){
            perror("Error while opening the employee file\n");
            return 0;
        }
        wr=write(newFd,&loan,sizeof(loan));
        close(newFd);
        //Updating
        if(update_loan(custId,loan.loan_id))
            return 1;
        return 0;
}


/*<------------------------------------------------------------------------------------->*/
//                  HAVE LOAN?
/*<------------------------------------------------------------------------------------->*/



int haveLoan(int id){
    char readBuffer[20];
    memset(readBuffer,0,sizeof(readBuffer));
    int fd,rd;
    int custid=id;
    // int id=custId;
    // int wr=write(connFD,"Enter amount to Withdraw",sizeof("Enter amount to Withdraw"));
    // rd=read(connFD,readBuffer,sizeof(readBuffer));
    struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        if(fd==-1){
            perror("Error while opening the file");
            return -1;
        }
        int os=lseek(fd,(custid-1000)*sizeof(struct customerDetails),0);
        if (os == EBADF) {
            perror("Error seeking in file_1");
            close(fd);
            return -1;
        }
        if (os == EINVAL) {
            perror("Error seeking in file_2");
            close(fd);
            return -1;
        }
        if (os == ENXIO) {
            perror("Error seeking in file_3");
            close(fd);
            return -1;
        }
        if (os == ENXIO) {
            perror("Error seeking in file_4");
            close(fd);
            return -1;
        }
        if (os == ESPIPE) {
            perror("Error seeking in file_5");
            close(fd);
            return -1;
        }
        //lock
        struct flock flk;
                flk.l_type=F_RDLCK;
                flk.l_len=sizeof(struct customerDetails);
                flk.l_pid=getpid();
                flk.l_start=0;
                flk.l_whence=(id-1000)*sizeof(cust);

                fcntl(fd,F_SETLKW,&flk);
                rd=read(fd,&cust,sizeof(cust));
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
        close(fd);
        if(cust.loan_status==0){
            return 1;
        }
        else return 0;
}


/*<------------------------------------------------------------------------------------->*/
//                  UPDATE LOAN
/*<------------------------------------------------------------------------------------->*/

bool update_loan(int account,char loanId[10]){
        int wr,rd,fd;
        struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(account-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return false;
                }
        struct flock flk;
                    flk.l_type=F_RDLCK;
                    flk.l_len=sizeof(struct customerDetails);
                    flk.l_pid=getpid();
                    flk.l_start=0;
                    flk.l_whence=(account-1000)*sizeof(cust);
                fcntl(fd,F_SETLKW,&flk);
                
                rd=read(fd,&cust,sizeof(cust));
                flk.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk);
        close(fd);
        cust.loan_status=1;
        strcpy(cust.loan_id,loanId);

        //Updating
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the customer file\n");
                    return false;
                }
                offset=lseek(fd,(account-1000)*sizeof(struct customerDetails),0);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return false;
                }
                struct flock lock;
                    lock.l_type=F_WRLCK;
                    lock.l_len=sizeof(cust);
                    lock.l_pid=getpid();
                    lock.l_start=0;
                    lock.l_whence=(account-1000)*sizeof(cust);
                    fcntl(fd,F_SETLKW,&lock);
                write(fd,&cust,sizeof(cust));
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
                close(fd);
        return true;
}



/*<------------------------------------------------------------------------------------->*/
//                  ASSIGN LOAN
/*<------------------------------------------------------------------------------------->*/


int assignLoan(int connFD){
    char readBuffer[30];
    char writeBuffer[1000];
    int wr,rd,fd;
    memset(writeBuffer,0,sizeof(writeBuffer));
    strcpy(writeBuffer,"Pendings Loans to Assign are:\n");
    struct loanDetails loan;
    fd=open(LOAN_FILE,O_RDONLY,0777);
    if(fd==-1){
        perror("Error while opening the file1");
        return -1;
    }
    struct flock flk;
                    flk.l_type=F_RDLCK;
                    // flk.l_len=sizeof(struct loanDetails);
                    flk.l_pid=getpid();
                    flk.l_start=0;
                    flk.l_whence=0;
                    fcntl(fd,F_SETLKW,&flk);
        // flk.l_type=F_UNLCK;
        // fcntl(fd,F_SETLKW,&flk);
    int i=1;
    // int os=lseek(fd,(i-1)*sizeof(struct loanDetails),0);
    int os=lseek(fd,(i-1)*sizeof(struct loanDetails),SEEK_SET);
    if(os==-1){
        perror("Error while seeking the file");
        close(fd);
        return -1;
    }
    read(fd,&loan,sizeof(loan));
    char t[100];
    strncpy(t,loan.loan_id+2,4);

    //Display all Loans in pending
    bool c=false;
    while(i==atoi(t)){
        if(loan.status==1){
            c=true;
            char temp[500];
            sprintf(temp,"\e[1mLoan Id: %s\e[m\nCustomer Account Number: %d\nAmount applied for Loan: %d\n\n",loan.loan_id,loan.cust_acc_num,loan.amount);
            strcat(writeBuffer,temp);
        }
        i++;
        os=lseek(fd,(i-1)*sizeof(struct loanDetails),SEEK_SET);
        if(os==-1){
            perror("Error while seeking the file");
            close(fd);
            return -1;
        }
        read(fd,&loan,sizeof(loan));
        memset(t,0,sizeof(t));
        strncpy(t,loan.loan_id+2,4);
    }
    flk.l_type=F_UNLCK;
    fcntl(fd,F_SETLKW,&flk);
    if(!c){
        memset(readBuffer,0,sizeof(readBuffer));
        strcat(writeBuffer,"NOTHING TO SHOW. ENTER 1 TO EXIT\n");
        wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
    }
    else{
        //Enter Loan Id
        memset(readBuffer,0,sizeof(readBuffer));
        strcat(writeBuffer,"Enter Loan Id to Assign:\n");
        wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        memset(t,0,sizeof(t));
        strncpy(t,readBuffer+2,4);

        os=lseek(fd,(atoi(t)-1)*sizeof(struct loanDetails),SEEK_SET);
        struct flock lock;
                        lock.l_type=F_RDLCK;
                        // flk.l_len=sizeof(struct loanDetails);
                        lock.l_pid=getpid();
                        lock.l_start=0;
                        lock.l_whence=0;
                        fcntl(fd,F_SETLKW,&lock);
            read(fd,&loan,sizeof(loan));
            lock.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&lock);

        while(strcmp(readBuffer,loan.loan_id)!=0 || loan.status!=1){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));

            strcat(writeBuffer,"Entered wrong Loan Id!\nReEnter Loan Id to Assign:\n");
            wr=write(connFD,writeBuffer,sizeof(writeBuffer));
            rd=read(connFD,readBuffer,sizeof(readBuffer));
            memset(t,0,sizeof(t));
            strncpy(t,readBuffer+2,4);

            os=lseek(fd,(atoi(t)-1)*sizeof(struct loanDetails),SEEK_SET);
            struct flock lock;
                            lock.l_type=F_RDLCK;
                            // flk.l_len=sizeof(struct loanDetails);
                            lock.l_pid=getpid();
                            lock.l_start=0;
                            lock.l_whence=0;
                            fcntl(fd,F_SETLKW,&lock);
                read(fd,&loan,sizeof(loan));
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
        }
        close(fd);
        

        //Enter employee id
        memset(readBuffer,0,sizeof(readBuffer));
        wr=write(connFD,"Enter Employee Id to Assign this Loan",sizeof("Enter Employee Id to Assign this Loan"));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        int empid=atoi(readBuffer);
        struct employeeDetails emp;
        int newFD=open(EMP_FILE,O_RDONLY,0777);
        int offset=lseek(newFD,(empid-1)*sizeof(struct employeeDetails),SEEK_SET);
        struct flock lock1;
                            lock1.l_type=F_RDLCK;
                            flk.l_len=sizeof(struct employeeDetails);
                            lock1.l_pid=getpid();
                            lock1.l_start=0;
                            lock1.l_whence=(empid-1)*sizeof(struct employeeDetails);
                            fcntl(fd,F_SETLKW,&lock1);
        rd=read(newFD,&emp,sizeof(emp));
        if(rd==-1){
            perror("Error while reading the file");
        }
                lock1.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock1);

        while(emp.id!=empid){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            wr=write(connFD," Entered wrong Employee Id!. ReEnter Employee Id to Assign this Loan",sizeof("Entered wrong Employee Id!. ReEnter Employee Id to Assign this Loan"));
            rd=read(connFD,readBuffer,sizeof(readBuffer));
            empid=atoi(readBuffer);
            // struct employeeDetails emp;
            // int newFD=open(EMP_FILE,O_RDWR,0777);
            offset=lseek(newFD,(empid-1)*sizeof(struct employeeDetails),SEEK_SET);
            if(offset==1){
                perror("Error while seeking");
                return -1;
            }
            struct flock lock1;
                                lock1.l_type=F_RDLCK;
                                flk.l_len=sizeof(struct employeeDetails);
                                lock1.l_pid=getpid();
                                lock1.l_start=0;
                                lock1.l_whence=(empid-1)*sizeof(struct employeeDetails);
                                fcntl(fd,F_SETLKW,&lock1);
            rd=read(newFD,&emp,sizeof(emp));
            if(rd==-1){
                perror("Error while readinf the file");
                return -1;
            }
                    lock1.l_type=F_UNLCK;
                    fcntl(fd,F_SETLKW,&lock1);
        }
        close(newFD);

        //Assignment of Loan
        emp.assignedLoans[emp.count]=atoi(t);
        emp.count=emp.count+1;
        newFD=open(EMP_FILE,O_WRONLY,0777);
        offset=lseek(newFD,(empid-1)*sizeof(struct employeeDetails),SEEK_SET);
        struct flock lock2;
                lock2.l_type=F_WRLCK;
                    lock2.l_len=sizeof(struct employeeDetails);
                    lock2.l_pid=getpid();
                    lock2.l_start=0;
                    lock2.l_whence=(empid-1)*sizeof(struct employeeDetails);
                    fcntl(fd,F_SETLKW,&lock2);
            write(newFD,&emp,sizeof(emp));
            lock2.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&lock2);
            close(newFD);

        
        fd=open(LOAN_FILE,O_WRONLY,0777);

        os=lseek(fd,(atoi(t)-1)*sizeof(struct loanDetails),SEEK_SET);
        if(os==-1){
            perror("error while seeking");
            return -1;
        }
        loan.status=4;
        loan.empId=empid;
        struct flock lock3;
                        lock3.l_type=F_WRLCK;
                        lock3.l_len=sizeof(struct loanDetails);
                        lock3.l_pid=getpid();
                        lock3.l_start=0;
                        lock3.l_whence=(atoi(t)-1)*sizeof(struct loanDetails);
                        fcntl(fd,F_SETLKW,&lock3);
            wr=write(fd,&loan,sizeof(loan));
            if(wr==-1){
                perror("Error while writing into loan file");
                return -1;

            }
            // rd=read(fd,&loan,sizeof(loan));
            lock3.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&lock3);
            close(fd);
            return 1;
    }
    
}



/*<------------------------------------------------------------------------------------->*/
//                  VIEW ASSIGN LOAN
/*<------------------------------------------------------------------------------------->*/

int view_assigned_loans(int connFD,int id,bool pending){
    char readBuffer[30];
    char writeBuffer[10000];
    char temp[500];
    struct loanDetails loan;
    int wr,rd;
    memset(writeBuffer,0,sizeof(writeBuffer));
    strcpy(writeBuffer,"Loans Assigned to You:\n\n");
    memset(readBuffer,0,sizeof(readBuffer));
    struct employeeDetails emp;
    int fd=open(EMP_FILE,O_RDONLY,0777);
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
                flk.l_whence=(id-1)*sizeof(struct employeeDetails);
            fcntl(fd,F_SETLKW,&flk);
            
            rd=read(fd,&emp,sizeof(emp));
            flk.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&flk);
    close(fd);

    int count=emp.count;
    fd=open(LOAN_FILE,O_RDONLY,0777);
    bool c=false;
    // memset(temp,0,sizeof(temp));
    for(int i=0;i<count;i++){
        int lid=emp.assignedLoans[i];
        if(lid!=-1){
            
            int os=lseek(fd,(lid-1)*sizeof(struct loanDetails),SEEK_SET);
            if(fd==-1){
                perror("Error while opening the file1");
                return -1;
            }
            struct flock flk1;
                            flk1.l_type=F_RDLCK;
                            flk1.l_len=sizeof(struct loanDetails);
                            flk1.l_pid=getpid();
                            flk1.l_start=0;
                            flk1.l_whence=(lid-1)*sizeof(struct loanDetails);
                            fcntl(fd,F_SETLKW,&flk1);
                rd=read(fd,&loan,sizeof(loan));
                flk1.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk1);
            if(pending && loan.status==4){
                memset(temp,0,sizeof(temp));
                sprintf(temp,"\e[1mLoan Id: %s\e[m\nCustomer Account Number: %d\nAmount applied for Loan: %d\n\e[1mStatus: Pending...\e[m\n",loan.loan_id,loan.cust_acc_num,loan.amount);
                strcat(writeBuffer,temp);
                c=true;
            }
            if(!pending && loan.status==2 ){
                memset(temp,0,sizeof(temp));
                sprintf(temp,"\e[1mLoan Id: %s\e[m\nCustomer Account Number: %d\nAmount applied for Loan: %d\n\e[1mStatus: Approved\e[m\n",loan.loan_id,loan.cust_acc_num,loan.amount);
                strcat(writeBuffer,temp);
                c=true;
            }
            if(!pending && loan.status==3){
                memset(temp,0,sizeof(temp));
                sprintf(temp,"\e[1mLoan Id: %s\e[m\nCustomer Account Number: %d\nAmount applied for Loan: %d\n\e[1mStatus: Rejected\e[m\n",loan.loan_id,loan.cust_acc_num,loan.amount);
                strcat(writeBuffer,temp);
                c=true;
            }
        }
    }
    close(fd);
    // strcat(writeBuffer,temp);
    // write(connFD,writeBuffer,sizeof(writeBuffer));
    // rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(c==false){
        return 3;
    }
    strcat(writeBuffer,"\n Enter 1 to exit\n");
    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    return 1;
}


/*<------------------------------------------------------------------------------------->*/
//                  ACCEPT REJECT LOAN
/*<------------------------------------------------------------------------------------->*/

int accept_reject_loan(int connFD,int id){
    char readBuffer[30];
    char writeBuffer[100];
    char temp[500];
    int wr,rd,fd;
    struct loanDetails loan;
    struct employeeDetails emp;

    //Employee read
    fd=open(EMP_FILE,O_RDONLY,0777);
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
            if(rd==-1){
                perror("Error while reading emp into file");
                close(fd);
                return -1;
            }
            flk.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&flk);
    close(fd);


    memset(writeBuffer,0,sizeof(writeBuffer));
    // memset(readBuffer,0,sizeof(readBuffer));
        // strcat(writeBuffer,"\nEnter Loan Id to Approve/Reject");
        wr=write(connFD,"\nEnter Loan Id to Approve/Reject",sizeof("\nEnter Loan Id to Approve/Reject"));
        memset(readBuffer,0,sizeof(readBuffer));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        char t[30];
        int lid;
        bool matched=false;
        memset(t,0,sizeof(t));
        strncpy(t,readBuffer+2,4);
        lid=atoi(t);

        fd=open(LOAN_FILE,O_RDONLY,0777);
            offset=lseek(fd,(lid-1)*sizeof(struct loanDetails),SEEK_SET);
            if(offset==-1){
                perror("error while seeking_1");
                return -1;
            }
            struct flock lock2;
                            lock2.l_type=F_RDLCK;
                            lock2.l_len=sizeof(struct loanDetails);
                            lock2.l_pid=getpid();
                            lock2.l_start=0;
                            lock2.l_whence=(lid-1)*sizeof(struct loanDetails);
                            fcntl(fd,F_SETLKW,&lock2);
                rd=read(fd,&loan,sizeof(loan));
                if(rd==-1){
                    perror("Error while writing into loan file");
                    return -1;

                }
                // rd=read(fd,&loan,sizeof(loan));
                lock2.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock2);
                close(fd);


        for(int i=0;i<emp.count;i++){
            if(emp.assignedLoans[i]==lid && loan.status==4){
                matched=true;
                break;
            }
        }

        while(!matched){
            wr=write(connFD,"Entered Wrong Loan Id!. Try Again or Enter 1 to exit!",sizeof("Entered Wrong Loan Id!. Try Again or Enter 1 to exit!"));
            memset(readBuffer,0,sizeof(readBuffer));
            rd=read(connFD,readBuffer,sizeof(readBuffer));
            if(atoi(readBuffer)==1){
                return 2;
            }
            memset(t,0,sizeof(t));
            strncpy(t,readBuffer+2,4);
            lid=atoi(t);

            memset(temp,0,sizeof(temp));
            strcpy(temp,readBuffer);

            for(int i=0;i<emp.count;i++){
                if(emp.assignedLoans[i]==lid){
                    int newfd=open(LOAN_FILE,O_RDONLY,0777);
                    offset=lseek(newfd,(lid-1)*sizeof(struct loanDetails),SEEK_SET);
                    if(offset==-1){
                        perror("error while seeking_1");
                        return -1;
                    }
                    struct flock lock1;
                                    lock2.l_type=F_RDLCK;
                                    lock1.l_len=sizeof(struct loanDetails);
                                    lock1.l_pid=getpid();
                                    lock1.l_start=0;
                                    lock1.l_whence=(lid-1)*sizeof(struct loanDetails);
                                    fcntl(newfd,F_SETLKW,&lock1);
                        rd=read(newfd,&loan,sizeof(loan));
                        if(rd==-1){
                            perror("Error while writing into loan file");
                            return -1;

                        }
                        // rd=read(fd,&loan,sizeof(loan));
                        lock1.l_type=F_UNLCK;
                        fcntl(newfd,F_SETLKW,&lock1);
                        close(newfd);
                    if(loan.status==4){
                        matched=true;
                        break;
                    }
                    if(matched) break;
                }
                if(matched) break;
            }
        }
        sprintf(writeBuffer,"Loan Id-> %d\n",lid);
        strcat(writeBuffer,"Enter \e[1m1\e[m to \e[1mAPPROVE LOAN\e[m or \e[1m0\e[m to \e[1mREJECT LOAN\e[m");
        memset(readBuffer,0,sizeof(readBuffer));
        wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        rd=read(connFD,readBuffer,sizeof(readBuffer));      
        int choice=atoi(readBuffer);
            
            
            if(choice==1){
                loan.status=2;
            }
            else{
                loan.status=3;
            }
            fd=open(LOAN_FILE,O_WRONLY,0777);
            offset=lseek(fd,(lid-1)*sizeof(struct loanDetails),SEEK_SET);
            struct flock lock3;
                            lock3.l_type=F_WRLCK;
                            lock3.l_len=sizeof(struct loanDetails);
                            lock3.l_pid=getpid();
                            lock3.l_start=0;
                            lock3.l_whence=(lid-1)*sizeof(struct loanDetails);
                            fcntl(fd,F_SETLKW,&lock3);
                wr=write(fd,&loan,sizeof(loan));
                if(wr==-1){
                    perror("Error while writing into loan file");
                    return -1;

                }
                // rd=read(fd,&loan,sizeof(loan));
                lock3.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock3);
                close(fd);

            //Customer Update
            struct customerDetails cust;
            fd=open(CUSTOMER_FILE,O_RDONLY,0777);
            //reading
            offset=lseek(fd,(loan.cust_acc_num-1000)*sizeof(struct customerDetails),SEEK_SET);
            if(offset==-1){
                perror("error while seeking_2");
                return -1;
            }
            struct flock lock4;
                            lock4.l_type=F_RDLCK;
                            lock4.l_len=sizeof(struct customerDetails);
                            lock4.l_pid=getpid();
                            lock4.l_start=0;
                            lock4.l_whence=(loan.cust_acc_num-1000)*sizeof(struct customerDetails);
                            fcntl(fd,F_SETLKW,&lock4);
                rd=read(fd,&cust,sizeof(cust));
                if(rd==-1){
                    perror("Error while writing into loan file");
                    return -1;

                }
                lock4.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock4);
                close(fd);


            if (offset == EBADF) {
            perror("Error seeking in file_1");
            close(fd);
            return -1;
            }
            if (offset == EINVAL) {
                perror("Error seeking in file_2");
                close(fd);
                return -1;
            }
            if (offset == ENXIO) {
                perror("Error seeking in file_3");
                close(fd);
                return -1;
            }
            if (offset == ENXIO) {
                perror("Error seeking in file_4");
                close(fd);
                return -1;
            }
            if (offset == ESPIPE) {
                perror("Error seeking in file_5");
                close(fd);
                return -1;
            }
            if(choice==1){
                cust.loan_status=2;
                cust.approval=1;
            }
            else{
                cust.loan_status=0;
                cust.approval=2;
            }
            fd=open(CUSTOMER_FILE,O_WRONLY,0777);
            offset=lseek(fd,(loan.cust_acc_num-1000)*sizeof(struct customerDetails),SEEK_SET);
            struct flock lock5;
                            lock5.l_type=F_WRLCK;
                            lock5.l_len=sizeof(struct customerDetails);
                            lock5.l_pid=getpid();
                            lock5.l_start=0;
                            lock5.l_whence=(loan.cust_acc_num-1000)*sizeof(struct customerDetails);
                            fcntl(fd,F_SETLKW,&lock5);
                wr=write(fd,&cust,sizeof(cust));
                if(wr==-1){
                    perror("Error while writing into loan file");
                    return -1;

                }
                lock5.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock5);
                close(fd);
                return 1;
}
#endif