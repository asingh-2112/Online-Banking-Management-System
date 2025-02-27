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
#include "./customer_struct.h"
#include "../LOAN/loan_functions.h"
#include "../TRANSACTIONS/transaction_struct.h"
#include "../TRANSACTIONS/transaction_func.h"
#include "../FEEDBACK/feedback_func.h"
#include "../FEEDBACK/feedback_struct.h"
#include "../COMMON/common.h"
#define CUSTOMER_FILE "./CUSTOMER/customer.txt"

// #define TRANS_FILE "../TRANSACTIONS/transaction.txt"
const char* salt = "sa";

int add_customer(int connFD);
bool modify_customer(int connFD);
int customer_operations(int connFD);
int customer_login(int connFD);
int view_cust_Balance(int connFD, int id);
int withdraw(int connFD);
int deposite(int connFD);
int transfer_funds(int connFD);
int add_feedback(int connFD,int id);
int view_feedback(int connFD);
bool change_password_cust(int connFD);
char custName[20];
int custId;
int sem_id;
struct sembuf sem_operation;



/*<------------------------------------------------------------------------------------->*/
//                  CUSTOMER OPERATIONS
/*<------------------------------------------------------------------------------------->*/




int customer_operations(int connFD){
    char readBuffer[500];
    char writeBuffer[500];
    char temp[100];
    int wr,rd,fd,attempts=3;
    int r=customer_login(connFD);
    if(r==0){ // Logged in Successfully
        sem_id = init_semphore(custId, CUSTOMER_FILE);
        lock(&sem_operation, sem_id);
            memset(writeBuffer,0,sizeof(writeBuffer));
        while(1){
            memset(readBuffer,0,sizeof(readBuffer));
            sprintf(writeBuffer,"Welcome %s\n",custName);
            strcat(writeBuffer,"Select from options to perform:\n\t1.View Account Balance\n\t2.Deposite Money\n\t3.Withdraw Money\n\t4.Transfer Funds\n\t5.Apply For Loan\n\t6.Change Password\n\t7.Adding Feedback\n\t8.View Transaction History\n\t9.Log out\n\t10.Exit\n");
            wr=write(connFD,writeBuffer,sizeof(writeBuffer));
            if(wr==-1){
                perror("Error while asking the options from employee");
                return -1;
            }
            // char tempBuffer[1024];
            // while (read(connFD, tempBuffer, sizeof(tempBuffer)) > 0); 
            rd=read(connFD,readBuffer,sizeof(readBuffer));
            if(rd==-1){
                perror("Error while taking the options from employee");
                return -1;
            }
            int choice=atoi(readBuffer);
            switch (choice)
            {
            case 1:
                /* code */
                int bal=view_cust_Balance(connFD,custId);
                memset(writeBuffer,0,sizeof(writeBuffer));
                sprintf(writeBuffer,"\e[1mBalance Availabe= %d\e[m\nEnter 1 to exit\n",bal);
                wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                break;

            case 2://Deposite amount
                bal=deposite(connFD);
                memset(writeBuffer,0,sizeof(writeBuffer));
                sprintf(writeBuffer,"\e[1mUpdated Balance= %d\e[m\nEnter 1 to exit\n",bal);
                wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                break;
            case 3://Withdraw amount
                bal=withdraw(connFD);
                memset(writeBuffer,0,sizeof(writeBuffer));
                sprintf(writeBuffer,"\e[1mUpdated Balance= %d\e[m\nEnter 1 to exit\n",bal);
                wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                break;
            case 4: //Funds_Transfer
                int res=transfer_funds(connFD);

                if(res==1){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    sprintf(writeBuffer,"\e[1mFunds transferred Successfully!\e[m\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else if(res==0){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    sprintf(writeBuffer,"\e[1Error While transferring !\e[m\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                break;
            case 5: //Apply for Loan
                res=apply_loan(connFD,true,custId);
                if(res==1){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Loan applied Successfully.It may take upto 2 to 3 working days for your loan to get approved by the bank!\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else if(res==2){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"You already have loan on your name\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else{
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Error while applying for Loan\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                break;
            case 6://Change Password
                if(change_password_cust(connFD)){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Password Changed Successfully\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else{
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Error while changing password\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                break;
            case 7: //Add feedback
                res=add_feedback(connFD,custId);
                if(res==1){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Added Feedback Successfully\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else{
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Error while adding feedback\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                break;
            case 8: //View Transaction history
                res=view_transaction(connFD,custId,true);
                if(res==1){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"All transaction viewed Successfully\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else if(res==2){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"No Transaction\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                else if(res==-1){
                    memset(writeBuffer,0,sizeof(writeBuffer));
                    strcpy(writeBuffer,"Error while viewing transaction\nEnter 1 to exit\n");
                    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
                    rd=read(connFD,readBuffer,sizeof(readBuffer));
                }
                break;
            case 9:
                unlock(&sem_operation, sem_id);
                return 0;
                break;
            default:
                unlock(&sem_operation, sem_id);
                break;
            }
        }
    return 0;
    }
    else if(r==1){ //Attempts Over
        return 1;
    }
    else if(r==2){//Attempts Over
        return 2;
    }
    else if(r==3){ //Deactivate
        return 3;
    }
    else{
        return -1;
    }
}


/*<------------------------------------------------------------------------------------->*/
//                  CUSTOMER LOGIN
/*<------------------------------------------------------------------------------------->*/


int customer_login(int connFD){
    char readBuffer[500];
    char writeBuffer[500];
    int wr,rd,fd,attempts=3;
    memset(readBuffer,0,sizeof(readBuffer));
    memset(writeBuffer,0,sizeof(writeBuffer));
    while(1){
        strcat(writeBuffer,"\nHello Customer! Enter your credentials:\n\nEnter you Account number");
        wr=write(connFD,writeBuffer,sizeof(writeBuffer));
        if(wr==-1){
            perror("Error while asking the id from Customer");
            return -1;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        int id=atoi(readBuffer);
        memset(readBuffer,0,sizeof(readBuffer));
        if(rd==-1){
            perror("Error while taking the id from Customer");
            return -1;
        }
        wr=write(connFD,"\nEnter your password",strlen("\nEnter your password"));
        if(wr==-1){
            perror("Error while asking the passowrd from Customer");
            return -1;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
            perror("Error while taking the passowrd from Customer");
            return -1;
        }

        struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
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

        if(cust.account_num!=id){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            attempts-=1;
            if(attempts)
                sprintf(writeBuffer,"\nCustomer Account Number doesn't exits. Attempts left- %d\n",attempts);
            else return 1;
        }
        else if(strcmp(cust.password,crypt(readBuffer,salt))!=0){
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            attempts-=1;
            if(attempts)
                sprintf(writeBuffer,"\nEntered wrong Password. Attempts left- %d\n",attempts);
            else return 2;
        }
        else if(!cust.active){
            return 3;
        }
        else{
            memset(writeBuffer,0,sizeof(writeBuffer));
            memset(readBuffer,0,sizeof(readBuffer));
            strcpy(custName,cust.name);
            custId=cust.account_num;
            return 0;
        }
    }
}


/*<------------------------------------------------------------------------------------->*/
//                  ADD CUSTOMER
/*<------------------------------------------------------------------------------------->*/

int add_customer(int connFD){
    char writeBuffer[1000];
    char readBuffer[1000];
    struct customerDetails cust,prevCust;
    int fd=open(CUSTOMER_FILE,O_RDONLY,0777);
    if(fd==-1 && errno==ENOENT){
        cust.account_num=1000;
    }
    else if(fd==-1){
        perror("Error in opening the file_1");
        return 0;
    }
    else {
        int offset=lseek(fd, -sizeof(struct customerDetails), SEEK_END);
        if (offset== -1) {
                    perror("Error seeking in file__2");
                    return 0;
        }
        struct flock flk;
                    flk.l_type=F_RDLCK;
                    flk.l_len=sizeof(struct customerDetails);
                    flk.l_pid=getpid();
                    flk.l_start=offset;
                    flk.l_whence=SEEK_SET;
                    fcntl(fd,F_SETLKW,&flk);
        read(fd,&prevCust,sizeof(prevCust));
        flk.l_type=F_UNLCK;
        fcntl(fd,F_SETLKW,&flk);
        close(fd);
        cust.account_num=prevCust.account_num+1;
    }
    strcpy(writeBuffer,"Enter Details of New Customer\n");

    //Customer Name
    strcat(writeBuffer,"\nEnter Customer Name");
    int wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    if(wr==-1){
        perror("Error in taking name as input\n");
        return 0;
    }
    memset(readBuffer,0,sizeof(readBuffer));
    int rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading name as input\n");
        return 0;
    }
    memset(cust.name,0,sizeof(cust.name));
    strcpy(cust.name,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Age
    wr=write(connFD,"\nEnter Customer's age",sizeof("Enter Customer's age"));
    if(wr==-1){
        perror("Error in taking age as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading age as input\n");
        return 0;
    }
    cust.age=atoi(readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Gender
    wr=write(connFD,"\nEnter Customer's gender",sizeof("Enter Customer's gender"));
    if(wr==-1){
        perror("Error in taking gender as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading gender as input\n");
        return 0;
    }
    strcpy(cust.gender,readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Balance
    wr=write(connFD,"\nEnter Customer's balance",sizeof("Enter Customer's balance"));
    if(wr==-1){
        perror("Error in taking balance as input\n");
        return 0;
    }
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading bakance as input\n");
        return 0;
    }
    cust.balance=atoi(readBuffer);
    memset(readBuffer,0,sizeof(readBuffer));

    //Password
    wr=write(connFD,"\nEnter Customer's Password",sizeof("Enter Customer's Password"));
    if(wr==-1){
        perror("Error in taking Password as input\n");
        return 0;
    }
    memset(readBuffer,0,sizeof(readBuffer));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error in reading Password as input\n");
        return 0;
    }
    memset(cust.password,0,sizeof(cust.password));
    strcpy(cust.password,crypt(readBuffer,salt));

    cust.active=true;
    cust.loan_status=false;
    cust.transcount=0;

    int newFd=open(CUSTOMER_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
    if(newFd==-1){
        perror("Error while opening the Customer file\n");
        return 0;
    }
    wr=write(newFd,&cust,sizeof(cust));
    close(newFd);

    return cust.account_num;
}


/*<------------------------------------------------------------------------------------->*/
//                  CUSTOMER DETAILS MODIFICATION
/*<------------------------------------------------------------------------------------->*/

bool modify_customer(int connFD){
    char writeBuffer[500];
    char readBuffer[500];
    
    while(1){
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        int wr,rd,fd;

        wr=write(connFD,"Enter account number of customer whose details is to be modified\n Or Enter exit to exit",sizeof("Enter account number of customer whose details is to be modified\n Or Enter exit to exit"));
        if(wr==-1){
            perror("Error while asking account number for modification\n");
            return false;
        }
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(rd==-1){
            perror("Error while taking account number for modification\n");
            return false;
        }
        if(strcmp(readBuffer,"exit")==0){
            return true;
        }
        int account=atoi(readBuffer);
        memset(readBuffer,0,sizeof(readBuffer));
        struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(account-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
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
        if(account==(cust.account_num) && cust.active){
            rechoose:
            sprintf(writeBuffer,"customer whose details are to be modified\n Account Number-> %d\n Name-> %s\n Age-> %d\n Gender-> %s\n Balance-> %d\n",cust.account_num,cust.name,cust.age,cust.gender,cust.balance);
            strcat(writeBuffer,"Select which detail to be modified of the customer\n");
            strcat(writeBuffer,"\t1.Name\n\t2.Age\n\t3.ReChoose Id of the customer\n\t4.Exit\n");
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
                strcpy(cust.name,readBuffer);
                fd=open(CUSTOMER_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the customer file\n");
                    return false;
                }
                offset=lseek(fd,(account-1000)*sizeof(struct customerDetails),0);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
                }
                struct flock flk;
                    flk.l_type=F_WRLCK;
                    flk.l_len=sizeof(cust);
                    flk.l_pid=getpid();
                    flk.l_start=0;
                    flk.l_whence=(account-1000)*sizeof(cust);
                    fcntl(fd,F_SETLKW,&flk);
                write(fd,&cust,sizeof(cust));
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
                // strcpy(cust.name,readBuffer);
                cust.age=atoi(readBuffer);
                fd=open(CUSTOMER_FILE,O_RDWR,0777);
                if(fd==-1){
                    perror("Error while opening the customer file\n");
                    return false;
                }
                offset=lseek(fd,(account-1000)*sizeof(struct customerDetails),0);
                if (offset == -1) {
                    perror("Error seeking in file");
                    close(fd);
                    return -1;
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
                strcpy(writeBuffer,"Age changed Successfully\n");
                break;
            case 3: //exit
                /* code */
                break;
            case 4: //exit
                /* code */
                return true;
                break;
            default:
                strcpy(writeBuffer,"Wrong option Entered.\n");
                goto rechoose;
            }
        }
        else{
            wr=write(connFD,"Wrong customer account entered",sizeof("Wrong customer account entered"));
            if(wr==-1){
                perror("Error in displaying Wrong customer account entered\n");
                return false;
            }
        }
    }
}


/*<------------------------------------------------------------------------------------->*/
//                  CUSTOMER vIEW BALANCE
/*<------------------------------------------------------------------------------------->*/

int view_cust_Balance(int connFD, int id){
    int fd,rd;
    struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return false;
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
        return cust.balance;
}


/*<------------------------------------------------------------------------------------->*/
//                  CUSTOMER DEPOSITE
/*<------------------------------------------------------------------------------------->*/

int deposite(int connFD){
    char readBuffer[20];
    memset(readBuffer,0,sizeof(readBuffer));
    int fd,rd;
    int id=custId;
    int wr=write(connFD,"Enter amount to Deposite",sizeof("Enter amount to Deposite"));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    int amt=atoi(readBuffer);
    struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDONLY,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return false;
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
        
        struct transactionDetails td,prevtd;
        int newFD=open(TRANS_FILE,O_RDONLY,0777);
        if(newFD==-1 && errno==ENOENT){
            td.transactionID=1;
        }
        else if(newFD==-1){
            perror("Error in opening the file_1");
            return 0;
        }
        else{
            offset=lseek(newFD, -sizeof(struct transactionDetails), SEEK_END);
            if (offset== -1) {
                        perror("Error seeking in file__2");
                        return 0;
            }
            struct flock flk1;
                        flk1.l_type=F_RDLCK;
                        flk1.l_len=sizeof(struct transactionDetails);
                        flk1.l_pid=getpid();
                        flk1.l_start=offset;
                        flk1.l_whence=SEEK_SET;
                        fcntl(newFD,F_SETLKW,&flk1);
            read(newFD,&prevtd,sizeof(prevtd));
            flk1.l_type=F_UNLCK;
            fcntl(newFD,F_SETLKW,&flk1);
            close(newFD);
            td.transactionID=prevtd.transactionID+1;
        }

        td.accountNumber=id;
        td.oldBalance=cust.balance;
        td.operation=1; //deposite

        cust.balance=cust.balance+atoi(readBuffer);
        td.newBalance=cust.balance;
        td.transAmount=amt;
        cust.transaction[cust.transcount%5]=td.transactionID;
        cust.transcount=cust.transcount+1;
        //writing in customer file
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        if(fd==-1){
            perror("Error while openeing file for updating the balance");
        }
        offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        struct flock lock;
                    lock.l_type=F_WRLCK;
                    lock.l_len=sizeof(cust);
                    lock.l_pid=getpid();
                    lock.l_start=0;
                    lock.l_whence=(id-1000)*sizeof(cust);
                    fcntl(fd,F_SETLKW,&lock);
                wr=write(fd,&cust,sizeof(cust));
                if(wr==-1){
                    perror("Error while writing into the file");
                    close(fd);
                    return -1;
                }
                td.transactionTime=time(NULL);
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
                close(fd);

            //Writing in transaction file
            newFD=open(TRANS_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
            if(newFD==-1){
            perror("Error in opening the file_3");
            close(newFD);
            return 0;
            }
            write(newFD,&td,sizeof(td));
            close(newFD);
            
        return cust.balance;
}


/*<------------------------------------------------------------------------------------->*/
//                  CUSTOMER WITHDRAW
/*<------------------------------------------------------------------------------------->*/


int withdraw(int connFD){
    char readBuffer[20];
    memset(readBuffer,0,sizeof(readBuffer));
    int fd,rd;
    int id=custId;
    int wr=write(connFD,"Enter amount to Withdraw",sizeof("Enter amount to Withdraw"));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    int amt=atoi(readBuffer);
    struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return false;
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
        
        struct transactionDetails td,prevtd;
        int newFD=open(TRANS_FILE,O_RDONLY,0777);
        if(newFD==-1 && errno==ENOENT){
            td.transactionID=1;
        }
        else if(newFD==-1){
            perror("Error in opening the file_1");
            return 0;
        }
        else{
            offset=lseek(newFD, -sizeof(struct transactionDetails), SEEK_END);
            if (offset== -1) {
                        perror("Error seeking in file__2");
                        return 0;
            }
            struct flock flk1;
                        flk1.l_type=F_RDLCK;
                        flk1.l_len=sizeof(struct transactionDetails);
                        flk1.l_pid=getpid();
                        flk1.l_start=offset;
                        flk1.l_whence=SEEK_SET;
                        fcntl(newFD,F_SETLKW,&flk1);
            read(newFD,&prevtd,sizeof(prevtd));
            flk1.l_type=F_UNLCK;
            fcntl(newFD,F_SETLKW,&flk1);
            close(newFD);
            td.transactionID=prevtd.transactionID+1;
        }

        td.accountNumber=id;
        td.oldBalance=cust.balance;
        td.operation=0; //deposite


        while(cust.balance-atoi(readBuffer)<0){
            memset(readBuffer,0,sizeof(readBuffer));
            wr=write(connFD,"Amount entered to withdraw is more than your Balance.\nEnter a lesser amount",sizeof("Amount entered to withdraw is more than your Balance.\nEnter a lesser amount"));
            rd=read(connFD,readBuffer,sizeof(readBuffer));
        }
        cust.balance=cust.balance-atoi(readBuffer);
        td.newBalance=cust.balance;
        td.transAmount=amt;
        cust.transaction[cust.transcount%5]=td.transactionID;
        cust.transcount=cust.transcount+1;

        fd=open(CUSTOMER_FILE,O_WRONLY,0777);
        if(fd==-1){
            perror("Error while openeing file for updating the balance");
        }
        offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        struct flock lock;
                    lock.l_type=F_WRLCK;
                    lock.l_len=sizeof(cust);
                    lock.l_pid=getpid();
                    lock.l_start=0;
                    lock.l_whence=(id-1000)*sizeof(cust);
                    fcntl(fd,F_SETLKW,&lock);
                wr=write(fd,&cust,sizeof(cust));
                if(wr==-1){
                    perror("Error while writing into the file");
                    close(fd);
                    return -1;
                }
                td.transactionTime=time(NULL);
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
                close(fd);

                //Writing in transaction file
                newFD=open(TRANS_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
                if(newFD==-1){
                perror("Error in opening the file_1");
                close(newFD);
                return 0;
                }
                write(newFD,&td,sizeof(td));
                close(newFD);

        return cust.balance;
}


/*<------------------------------------------------------------------------------------->*/
//                  ACTIVATE/DEACTIVATE
/*<------------------------------------------------------------------------------------->*/

bool activeAccount(int connFD){
    char readBuffer[20];
    char writeBuffer[200];
    memset(readBuffer,0,sizeof(readBuffer));
    memset(writeBuffer,0,sizeof(writeBuffer));
    int fd,rd;
    int wr=write(connFD,"Enter Account Number of the Customer",sizeof("Enter Account Number of the Customer"));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    int id=atoi(readBuffer);
    struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return false;
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
    memset(readBuffer,0,sizeof(readBuffer));
    memset(writeBuffer,0,sizeof(writeBuffer));
    if(cust.active){
        sprintf(writeBuffer,"Account holder's Name-> %s\nStatus-> \e[1mACTIVE\e[m\nType YES to DEACTIVATE\nType NO to exit",cust.name);
    }
    else{
        sprintf(writeBuffer,"Account holder's Name-> %s\nStatus-> \e[1mDEACTIVE\e[m\nType YES to ACTIVATE\nType NO to exit",cust.name);
    }
    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(strcmp(readBuffer,"YES")==0 ||strcmp(readBuffer,"yes")==0 ||strcmp(readBuffer,"Yes")==0){
        cust.active=!cust.active;

        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        if(fd==-1){
            perror("Error while openeing file for updating the balance");
        }
        offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        struct flock lock;
                    lock.l_type=F_WRLCK;
                    lock.l_len=sizeof(cust);
                    lock.l_pid=getpid();
                    lock.l_start=0;
                    lock.l_whence=(id-1000)*sizeof(cust);
                    fcntl(fd,F_SETLKW,&lock);
                write(fd,&cust,sizeof(cust));
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
                close(fd);

        return true;
    }
    return false;
}



/*<------------------------------------------------------------------------------------->*/
//                  TRANSFER FUNDS
/*<------------------------------------------------------------------------------------->*/

int transfer_funds(int connFD){
    char readBuffer[20];
    char writeBuffer[100];
    memset(readBuffer,0,sizeof(readBuffer));
    int fd,rd,recvId,wr;
    bool exit=false;
    int id=custId;
    struct customerDetails cust,recvCust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return 0;
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
            // Recipient
        memset(readBuffer,0,sizeof(readBuffer));
        wr=write(connFD," Enter Account Number of Recipient. Enter exit to EXIT",sizeof(" Enter Account Number of Recipient. Enter exit to EXIT"));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        if(strcmp(readBuffer,"exit")==0){
            return 2;
        }
        recvId=atoi(readBuffer);
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int os=lseek(fd,(recvId-1000)*sizeof(struct customerDetails),0);
        if (os == -1) {
            perror("Error seeking in file");
            close(fd);
            return 0;
        }
        struct flock lock;
                lock.l_type=F_RDLCK;
                lock.l_len=sizeof(struct customerDetails);
                lock.l_pid=getpid();
                lock.l_start=0;
                lock.l_whence=(recvId-1000)*sizeof(cust);

                fcntl(fd,F_SETLKW,&lock);
                rd=read(fd,&recvCust,sizeof(cust));
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
        close(fd);

    while(recvCust.account_num!=recvId || !recvCust.active){
        memset(readBuffer,0,sizeof(readBuffer));
        wr=write(connFD," Entered Wrong Account Number of Recipient !\n Enter Again Account Number of Recipient",sizeof(" Entered Wrong Account Number of Recipient !\n Enter Again Account Number of Recipient"));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        recvId=atoi(readBuffer);
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        offset=lseek(fd,(recvId-1000)*sizeof(struct customerDetails),0);
        if (offset == -1) {
            perror("Error seeking in file");
            close(fd);
            return 0;
        }
        struct flock lock;
                lock.l_type=F_RDLCK;
                lock.l_len=sizeof(struct customerDetails);
                lock.l_pid=getpid();
                lock.l_start=0;
                lock.l_whence=(recvId-1000)*sizeof(cust);

                fcntl(fd,F_SETLKW,&lock);
                rd=read(fd,&recvCust,sizeof(cust));
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
        close(fd);
    }
    memset(readBuffer,0,sizeof(readBuffer));
    memset(writeBuffer,0,sizeof(writeBuffer));
    sprintf(writeBuffer,"Recipient's Name: %s\n Enter Amount to be Deposited:",recvCust.name);
    wr=write(connFD,writeBuffer,sizeof(writeBuffer));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    int amt=atoi(readBuffer);
    memset(writeBuffer,0,sizeof(writeBuffer));

    while(cust.balance-atoi(readBuffer)<0){
        memset(readBuffer,0,sizeof(readBuffer));
        wr=write(connFD," Balance is Low!\n Enter amount Again",sizeof(" Balance is Low.\n Enter amount Again"));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
    }

    //Transaction
    struct transactionDetails td,prevtd,recvtd;
        int newFD=open(TRANS_FILE,O_RDONLY,0777);
        if(newFD==-1 && errno==ENOENT){
            td.transactionID=1;
        }
        else if(newFD==-1){
            perror("Error in opening the file_1");
            return 0;
        }
        else{
            offset=lseek(newFD, -sizeof(struct transactionDetails), SEEK_END);
            if (offset== -1) {
                        perror("Error seeking in file__2");
                        return 0;
            }
            struct flock flk1;
                        flk1.l_type=F_RDLCK;
                        flk1.l_len=sizeof(struct transactionDetails);
                        flk1.l_pid=getpid();
                        flk1.l_start=offset;
                        flk1.l_whence=SEEK_SET;
                        fcntl(newFD,F_SETLKW,&flk1);
            read(newFD,&prevtd,sizeof(prevtd));
            flk1.l_type=F_UNLCK;
            fcntl(newFD,F_SETLKW,&flk1);
            close(newFD);
            td.transactionID=prevtd.transactionID+1;
        }

        td.accountNumber=id;
        td.oldBalance=cust.balance;
        td.operation=2; //Transfered To
        td.transferedToFrom=recvId;
        memset(td.receipentName,0,sizeof(td.receipentName));
        strcpy(td.receipentName,recvCust.name);

    //Balance deducting from sender's
        cust.balance=cust.balance-atoi(readBuffer);
        td.newBalance=cust.balance;
        td.transAmount=amt;
        cust.transaction[cust.transcount%5]=td.transactionID;
        cust.transcount=cust.transcount+1;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        if(fd==-1){
            perror("Error while openeing file for updating the balance");
            close(fd);
            return 0;
        }
        offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),0);
        struct flock lock1;
                    lock1.l_type=F_WRLCK;
                    lock1.l_len=sizeof(cust);
                    lock1.l_pid=getpid();
                    lock1.l_start=0;
                    lock1.l_whence=(id-1000)*sizeof(cust);
                    fcntl(fd,F_SETLKW,&lock1);
                write(fd,&cust,sizeof(cust));
                td.transactionTime=time(NULL);
                lock1.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock1);
                // close(fd);
                //Writing in transaction file
            newFD=open(TRANS_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
            if(newFD==-1){
            perror("Error in opening the file_1");
            close(newFD);
            return 0;
            }
            write(newFD,&td,sizeof(td));
            close(newFD);

            newFD=open(TRANS_FILE,O_RDONLY,0777);
            if(newFD==-1 && errno==ENOENT){
                recvtd.transactionID=1;
            }
            else if(newFD==-1){
                perror("Error in opening the file_1");
                return 0;
            }
            else{
                offset=lseek(newFD, -sizeof(struct transactionDetails), SEEK_END);
                if (offset== -1) {
                            perror("Error seeking in file__2");
                            return 0;
                }
                struct flock flk1;
                            flk1.l_type=F_RDLCK;
                            flk1.l_len=sizeof(struct transactionDetails);
                            flk1.l_pid=getpid();
                            flk1.l_start=offset;
                            flk1.l_whence=SEEK_SET;
                            fcntl(newFD,F_SETLKW,&flk1);
                read(newFD,&prevtd,sizeof(prevtd));
                flk1.l_type=F_UNLCK;
                fcntl(newFD,F_SETLKW,&flk1);
                close(newFD);
                recvtd.transactionID=prevtd.transactionID+1;
            }

            recvtd.accountNumber=recvId;
            recvtd.oldBalance=recvCust.balance;
            recvtd.operation=3; //Transfered From
            recvtd.transferedToFrom=id;
            memset(recvtd.receipentName,0,sizeof(td.receipentName));
            strcpy(recvtd.receipentName,cust.name);
    //Amount adding into recipient's account
        recvCust.balance=recvCust.balance+atoi(readBuffer);
        recvtd.newBalance=recvCust.balance;
        recvtd.transAmount=amt;
        recvCust.transaction[recvCust.transcount%5]=recvtd.transactionID;
        recvCust.transcount=recvCust.transcount+1;
        offset=lseek(fd,(recvId-1000)*sizeof(struct customerDetails),0);
        struct flock lock2;
                    lock2.l_type=F_WRLCK;
                    lock2.l_len=sizeof(recvCust);
                    lock2.l_pid=getpid();
                    lock2.l_start=0;
                    lock2.l_whence=(recvId-1000)*sizeof(recvCust);
                    fcntl(fd,F_SETLKW,&lock2);
                write(fd,&recvCust,sizeof(cust));
                recvtd.transactionTime=time(NULL);
                lock2.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock2);
                close(fd);

                //Writing receiving to trans file
                newFD=open(TRANS_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
                if(newFD==-1){
                perror("Error in opening the file_1");
                close(newFD);
                return 0;
                }
                write(newFD,&recvtd,sizeof(recvtd));
                close(newFD);
                return 1;

}


/*<------------------------------------------------------------------------------------->*/
//                  CHANGE PASSWORD
/*<------------------------------------------------------------------------------------->*/

bool change_password_cust(int connFD){
    char writeBuffer[500];
    char readBuffer[500];
    while(1){
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        int wr,rd,fd;
        int id=custId;
        struct customerDetails cust;
        fd=open(CUSTOMER_FILE,O_RDWR,0777);
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),SEEK_SET);
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
                    flk.l_whence=(id-1000)*sizeof(cust);
                fcntl(fd,F_SETLKW,&flk);
                
                rd=read(fd,&cust,sizeof(cust));
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
                if(strcmp(cust.password,crypt(readBuffer,salt))!=0){
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
                        strcpy(cust.password,crypt(readBuffer,salt));
                        fd=open(CUSTOMER_FILE,O_RDWR,0777);
                        if(fd==-1){
                            perror("Error while opening the employee file\n");
                            return false;
                        }
                        offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),SEEK_SET);
                        if (offset == -1) {
                            perror("Error seeking in file");
                            close(fd);
                            return -1;
                        }
                        struct flock flk;
                            flk.l_type=F_WRLCK;
                            flk.l_len=sizeof(cust);
                            flk.l_pid=getpid();
                            flk.l_start=0;
                            flk.l_whence=(id-1000)*sizeof(cust);
                            fcntl(fd,F_SETLKW,&flk);
                        write(fd,&cust,sizeof(cust));
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