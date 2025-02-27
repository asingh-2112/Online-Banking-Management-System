#ifndef FEEDBACK_DETAILS
#define FEEDBACK_DETAILS

#include <time.h>

struct feedbackDetails
{
    int feedbackID; // 0, 1, 2, 3 ...
    int accountNumber;
    bool action; // 0 -> Posted, 1 -> Action taken
    char feed[50];
    char remark[50];
    time_t feedTime;
};


#endif