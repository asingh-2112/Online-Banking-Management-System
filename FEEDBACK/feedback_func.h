#ifndef FEED_DETAILS
#define FEED_DETAILS
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
#include "../FEEDBACK/feedback_struct.h"
// #define CUST_FILE "../CUSTOMER/customer.txt"
#define FEED_FILE "./FEEDBACK/feedback.txt"

int add_feedback(int connFD,int id);
int view_feedback(int connFD);


/*<------------------------------------------------------------------------------------->*/
//                  ADD FEEDBACK
/*<------------------------------------------------------------------------------------->*/

int add_feedback(int connFD,int id){
    char writeBuffer[100];
    char readBuffer[100];
    int wr,rd,fd;
    memset(readBuffer,0,sizeof(readBuffer));
    wr=write(connFD,"Type yout feedback (Not more than 50 characters including spaces)",sizeof("Type yout feedback (Not more than 50 characters including spaces)"));
    rd=read(connFD,readBuffer,sizeof(readBuffer));
    if(rd==-1){
        perror("Error while reading feedback");
        return -1;
    }
    struct feedbackDetails feed,prevfd;
        int newFD=open(FEED_FILE,O_RDONLY,0777);
        if(newFD==-1 && errno==ENOENT){
            feed.feedbackID=1;
            close(newFD);
        }
        else if(newFD==-1){
            perror("Error in opening the file_1");
            close(newFD);
            return -1;
        }
        else{
            int offset=lseek(newFD, -sizeof(struct feedbackDetails), SEEK_END);
            if (offset== -1) {
                        perror("Error seeking in file__2");
                        return -1;
            }
            struct flock flk1;
                        flk1.l_type=F_RDLCK;
                        flk1.l_len=sizeof(struct feedbackDetails);
                        flk1.l_pid=getpid();
                        flk1.l_start=offset;
                        flk1.l_whence=SEEK_SET;
                        fcntl(newFD,F_SETLKW,&flk1);
            read(newFD,&prevfd,sizeof(prevfd));
            flk1.l_type=F_UNLCK;
            fcntl(newFD,F_SETLKW,&flk1);
            close(newFD);
            feed.feedbackID=prevfd.feedbackID+1;
        }
    feed.accountNumber=id;
    feed.action=false;
    strcpy(feed.feed,readBuffer);
    feed.feedTime=time(NULL);
    
    newFD=open(FEED_FILE,O_CREAT |O_WRONLY | O_APPEND,S_IRWXU);
            if(newFD==-1){
            perror("Error in opening the file_3");
            close(newFD);
            return -1;
            }
            write(newFD,&feed,sizeof(feed));
            close(newFD);
    //Updating customert
    struct customerDetails cust;
    fd=open(CUST_FILE,O_RDONLY,0777);
    int offset=lseek(fd,(id-1000)*sizeof(struct customerDetails),SEEK_SET);
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
    
    cust.feedbackId[cust.feedBackCount%5]=feed.feedbackID;
    cust.feedBackCount++;

    fd=open(CUST_FILE,O_WRONLY,0777);
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
                lock.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&lock);
                close(fd);
                return 1;
}


/*<------------------------------------------------------------------------------------->*/
//                  VIEW FEEDBACK
/*<------------------------------------------------------------------------------------->*/

int view_feedback(int connFD){
    char writeBuffer[1000];
    char readBuffer[200];
    char temp[500];
    int rd,wr,fd;
    bool isEmpty=false;
    int i=1;
    int offset=0;
    struct feedbackDetails feed;
    fd=open(FEED_FILE,O_RDONLY,0777);
    offset=lseek(fd,(i-1)*sizeof(struct feedbackDetails),SEEK_SET);
    if(offset==-1){
            perror("Error while seeking the file1");
            close(fd);
            return -1;
        }
    struct flock flk1;
                    flk1.l_type=F_RDLCK;
                    flk1.l_len=sizeof(struct feedbackDetails);
                    flk1.l_pid=getpid();
                    flk1.l_start=offset;
                    flk1.l_whence=SEEK_SET;
                    fcntl(fd,F_SETLKW,&flk1);
            read(fd,&feed,sizeof(feed));
            flk1.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&flk1);
            close(fd);
    
    while(feed.feedbackID==i){
        memset(temp,0,sizeof(temp));
        memset(writeBuffer,0,sizeof(writeBuffer));
        memset(readBuffer,0,sizeof(readBuffer));
        if(feed.action==false){
            isEmpty=true;
            sprintf(temp," Account No: %d\n Time: %s Feedback ID: FD%04d\n Feedback:\n ",feed.accountNumber,ctime(&feed.feedTime),feed.feedbackID);
            strcat(temp,feed.feed);
            strcat(temp,"\nEnter 1 to add remark. Or enter 2 to go to next feedback. Or enter 0 to exit");
            wr=write(connFD,temp,sizeof(temp));
            rd=read(connFD,readBuffer,sizeof(readBuffer));
            int choice=atoi(readBuffer);
            memset(readBuffer,0,sizeof(readBuffer));
            if(choice==0){
                break;
            }
            else if(choice==1){
                wr=write(connFD,"Add Remark(Not more than 50 characters):",sizeof("Add Remark(Not more than 50 characters):"));
                rd=read(connFD,readBuffer,sizeof(readBuffer));
                strcpy(feed.remark,readBuffer);
                feed.action=true;
                fd=open(FEED_FILE,O_WRONLY,0777);
                offset=lseek(fd,(i-1)*sizeof(struct feedbackDetails),SEEK_SET);
                struct flock flk2;
                                flk2.l_type=F_WRLCK;
                                flk2.l_len=sizeof(struct feedbackDetails);
                                flk2.l_pid=getpid();
                                flk2.l_start=offset;
                                flk2.l_whence=SEEK_SET;
                                fcntl(fd,F_SETLKW,&flk2);
                        write(fd,&feed,sizeof(feed));
                        flk2.l_type=F_UNLCK;
                        fcntl(fd,F_SETLKW,&flk2);
                        close(fd);
            }

        }
        i++;
        memset(feed.feed,0,sizeof(feed.feed));
        memset(feed.remark,0,sizeof(feed.remark));
        fd=open(FEED_FILE,O_RDONLY,0777);
        offset=lseek(fd,(i-1)*sizeof(struct feedbackDetails),SEEK_SET);
        if(offset==-1){
            perror("Error while seeking the file");
            close(fd);
            return -1;
        }
        struct flock flk1;
                        flk1.l_type=F_RDLCK;
                        flk1.l_len=sizeof(struct feedbackDetails);
                        flk1.l_pid=getpid();
                        flk1.l_start=offset;
                        flk1.l_whence=SEEK_SET;
                        fcntl(fd,F_SETLKW,&flk1);
                read(fd,&feed,sizeof(feed));
                flk1.l_type=F_UNLCK;
                fcntl(fd,F_SETLKW,&flk1);
                close(fd);
    }
    if(isEmpty==false){
        return 2;
    }
    return 1;
}

#endif
