#ifndef LOAN_DETAILS
#define LOAN_DETAILS
struct loanDetails{
    char loan_id[10];
    
    int status; //0-> nothing, 1->Pending, 2->Approved, 3-> Rejected, 4->Assigned
    bool complete;
    int cust_acc_num;
    int amount;
    int empId;
};
#endif 