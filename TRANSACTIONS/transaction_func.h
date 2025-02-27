#ifndef TRANS_DETAILS
#define TRANS_DETAILS
#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "../CUSTOMER/customer_struct.h"
#define CUST_FILE "./CUSTOMER/customer.txt"
#define TRANS_FILE "./TRANSACTIONS/transaction.txt"
int view_transaction(int connFD,int id, bool isCust);


/*<------------------------------------------------------------------------------------->*/
//                  VIEW TRANSACTION
/*<------------------------------------------------------------------------------------->*/

int view_transaction(int connFD,int id, bool isCust){
    char readBuffer[30];
    char writeBuffer[1000];
    char temp[500];
    char buf[50];
    int wr,rd;
    bool isTrans=false;
    int count=0;
    memset(readBuffer,0,sizeof(readBuffer));
    memset(writeBuffer,0,sizeof(writeBuffer));
    if(!isCust){
        wr=write(connFD,"Enter Customer Account Number",sizeof("Enter Customer Account Number"));
        rd=read(connFD,readBuffer,sizeof(readBuffer));
        id=atoi(readBuffer);
    }
        //reading of customer to struct
        struct customerDetails cust;
        int fd=open(CUST_FILE,O_RDONLY,0777);
        if(fd==-1){
            perror("Error while opening the file");
            close(fd);
            return -1;
        }
        int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),SEEK_SET);
        if(offset==-1){
            close(fd);
            return -1;
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
                fcntl(fd,F_SETLK);
                close(fd);
        // memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer)); 
        fd=open(TRANS_FILE,O_RDONLY,0777);
        memset(temp,0,sizeof(temp));
        if(cust.transcount<5){
            for(int i=0;i<cust.transcount;i++){
                struct transactionDetails trans;
                int tid=cust.transaction[i];
                offset=lseek(fd,(tid-1)*sizeof(struct transactionDetails),SEEK_SET);
                struct flock flk1;
                flk1.l_type=F_RDLCK;
                flk1.l_len=sizeof(struct transactionDetails);
                flk1.l_pid=getpid();
                flk1.l_start=0;
                flk1.l_whence=(tid-1)*sizeof(struct transactionDetails);

                fcntl(fd,F_SETLKW,&flk1);
                rd=read(fd,&trans,sizeof(trans));
                flk1.l_type=F_UNLCK;
                fcntl(fd,F_SETLK);
                if(trans.operation==0){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Withdrawal\n Transaction Amount: %d\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                else if(trans.operation==1){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Deposite\n Transaction Amount: %d\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                else if(trans.operation==2){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Transfered\n Transaction Amount: %d\n Transfered To: %d (%s)\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.transferedToFrom,trans.receipentName,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                else if(trans.operation==3){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Received\n Transaction Amount: %d\n Received From: %d (%s)\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.transferedToFrom,trans.receipentName,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                isTrans=true;
                strcat(writeBuffer,temp);
                memset(temp,0,sizeof(temp));
            }
            close(fd);
        }
        else{
            memset(writeBuffer,0,sizeof(writeBuffer));
            int start = cust.transcount;
            
            for(int i=0;i<5;i++){
                struct transactionDetails trans;
                int tid=cust.transaction[start%5];
                offset=lseek(fd,(tid-1)*sizeof(struct transactionDetails),SEEK_SET);
                struct flock flk1;
                flk1.l_type=F_RDLCK;
                flk1.l_len=sizeof(struct transactionDetails);
                flk1.l_pid=getpid();
                flk1.l_start=0;
                flk1.l_whence=(tid-1)*sizeof(struct transactionDetails);

                fcntl(fd,F_SETLKW,&flk1);
                rd=read(fd,&trans,sizeof(trans));
                flk1.l_type=F_UNLCK;
                fcntl(fd,F_SETLK);
                if(trans.operation==0){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Withdrawal\n Transaction Amount: %d\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                else if(trans.operation==1){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Deposite\n Transaction Amount: %d\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                else if(trans.operation==2){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Transfered\n Transaction Amount: %d\n Transfered To: %d (%s)\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.transferedToFrom,trans.receipentName,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                else if(trans.operation==3){
                    sprintf(temp,"\e[1m Transaction Id: TN%04d\e[m\n Operation Type: Received\n Transaction Amount: %d\n Received From: %d (%s)\n Old Balance: %d\n New Balance: %d\n Transaction Time: %s\n",trans.transactionID,trans.transAmount,trans.transferedToFrom,trans.receipentName,trans.oldBalance,trans.newBalance,ctime(&trans.transactionTime));
                }
                isTrans=true;
                strcat(writeBuffer,temp);
                memset(temp,0,sizeof(temp));
                start++;
            }
            close(fd);
        }
        
        if(!isTrans) {
            return 2; // No transactions
        }

memset(readBuffer, 0, sizeof(readBuffer));
wr = write(connFD, writeBuffer, sizeof(writeBuffer));
rd = read(connFD, readBuffer, sizeof(readBuffer));
if(atoi(readBuffer)==1)
    return 1;  // Exit successfully

}



#endif


