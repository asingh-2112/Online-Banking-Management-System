#ifndef CUSTOMER_DETAILS
#define CUSTOMER_DETAILS
struct customerDetails{
    bool active;
    int account_num;
    char name[30];
    int age;
    char gender[10];
    int balance;
    int loan_status; //0 -> nothing 1->applied 2->Action Taken
    int approval; // 0->nothing 1->approved 2->rejected;
    char loan_id[10];
    char password[30];
    int transcount;
    int transaction[5];
    int feedBackCount;
    int feedbackId[5];
};
#endif 