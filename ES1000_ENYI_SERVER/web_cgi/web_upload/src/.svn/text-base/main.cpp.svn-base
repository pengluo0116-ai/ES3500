#include "main.h"
using namespace std;

#if 1
int main(){
    try{
        get_uploadData();
    }catch(...){ err_msg("访问有误，请稍候再试");}
    
    return 0;
}
#endif

#if 0
#define MAX_CONTENT_LEN        1024

int main()
{
    extern char   **environ;
    int nlen = 0;
    int i;
    char szContent[MAX_CONTENT_LEN];
    char **penv;
    char *req = NULL;

    memset(szContent, 0, MAX_CONTENT_LEN);
        
    printf("Content-type: text/html\n\n");
    
    for ( penv = environ; *penv; penv++ )
        printf("%s<br>", *penv);

    if ( strcmp("POST", getenv("REQUEST_METHOD")) == 0 )
    {
        nlen = atoi(getenv("CONTENT_LENGTH"));
        for (i = 0; i < nlen; i++ )    
        {
            if ( i < MAX_CONTENT_LEN )
                szContent[i] = fgetc(stdin);
            else
                break;
        }
        printf("<p>%s</p>", szContent);
    }
    else
    {
        printf("<p>%s</p>", getenv("QUERY_STRING"));
    }
    return 0;
}
#endif