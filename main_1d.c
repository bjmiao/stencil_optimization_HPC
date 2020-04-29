#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
// 1D 3-point stencil

void stencil(double a[], int N, int tmax)
{
    int i;
    // double *a_old;
    // a_old = (double*)malloc(N * sizeof(double));
    // memcpy(a_old, a, N*sizeof(double));
    // double *p_old = a;
    // double *p_new = a;
    // printf("%p %p\n", a, a_old);

    while (tmax--){
        for (i=1; i<N-1; i++)
        {
            a[i] = (a[i-1] + a[i+1]) / 2;
            // p_new[i] = (p_old[i-1] + p_old[i+1]) / 2;
        }
        // double *temp = p_old; p_old = p_new; p_new = temp;
        // printf("%p %p\n", p_old, p_new);
    }
    // p_old is the last result due to the swap operation
    // if (p_old != a)
        // memcpy(a, a_old, N * sizeof(double));
    
    // free(a_old);
    return;
}

inline kernel(int queue[], int x)
{
    static int i=0;
    queue[i++] = x;
    // int this_ptr = N*t0+x;
    // if (x==0 || x == N-1) 
    //     a_all[N*t0+x] = a_all[N*(t0-1)+x];
    // else 
    //     a_all[N*t0+x] = (a_all[N*(t0-1) + x-1] + a_all[N*(t0-1) + x+1])/2;
    // printf("kernel (x,t)=(%d, %d), %lf\n", x, t0, a_all[N*t0+x]);

}

void walk(int queue[], int t0, int t1, int x0, int dx0, int x1, int dx1, int N, int tmax)
{
    // printf("Walk %d %d %d %d %d %d\n", t0, t1, x0, dx0, x1, dx0);
    int dt = t1-t0;

    if (dt == 1){
        // base case
        int x;
        for (x=x0; x<x1; x++) kernel(queue, x);
    }
    else if (dt > 1){
        if (2 * (x1-x0) + (dx1 - dx0) * dt >= 4 * dt){
            // space cut
            int xm = (2 * (x0 + x1) + (2 + dx0 + dx1) * dt) /4;
            walk(queue, t0, t1, x0, dx0, xm, -1, N, tmax);
            walk(queue, t0, t1, xm, -1, x1, dx1, N, tmax);
        } else {
            // time cut
            int s = dt / 2;
            walk(queue, t0, t0+s, x0, dx0, x1, dx1, N, tmax);
            walk(queue, t0+s, t1, x0 + dx0*s, dx0, x1 + dx1 * s, dx1, N, tmax);
        }
    }
}

void stencil_opt(double a[], int N, int tmax_all)
{
    int *queue;
    int tmax = tmax_all / 10;
    queue = (int *) malloc(N*tmax*sizeof(int));
    // memcpy(queue, a, sizeof(double)*N);
    char queue_filename[1024];
    sprintf(queue_filename, "queue_n%d_t%d.txt", N, tmax);
    FILE *fp;
    int i;
    // fp = fopen(queue_filename, "r");
    // if (fp){
    //     printf("Load pre-calculated queue\n");
    //     // has pre-calculated queue
    //     for (i=0; i<N*tmax; i++) fscanf(fp, "%d", &queue[i]);
    //     fclose(fp);
    // } else {
        // printf("Not found pre-calculated queue, calculating new one\n");
        walk(queue, 1, tmax+1, 0, 0, N, 0, N, tmax);
    //     printf("Store this queue\n");
    //     fp = fopen(queue_filename, "w");
    //     for (i=0; i<N*tmax; i++) fprintf(fp, "%d ", queue[i]);
    //     fprintf(fp, "\n");
    //     fclose(fp);
    // }
    printf("Start calculating\n");
    int ntimes;
    for (ntimes=0; ntimes<10; ntimes++)
    for (i=0; i<N*tmax; i++) {
        int x=queue[i];
        // printf("%d ", x);
        if (x == 0 || x == N-1)
            continue;
        else 
            a[x] = (a[x-1] + a[x+1])/2;
    }
    // printf("\n");
    
    // for (i=0; i<100; i++) printf("%lf ", a[i]);
    // printf("\n");
    // memcpy(a, a_all + tmax*N, sizeof(double)*N);
    free(queue);
    return;
}



int main(int argc, char* argv[])
{

    FILE *fp;
    fp = fopen("size.txt","r");
    if (!fp) {printf("Input file not exist\n"); exit(1);}
    int N, tmax;
    fscanf(fp, "%d%d", &N, &tmax);
    fclose(fp);
    double *a;
    a = (double*) malloc(N * sizeof(double));

    srand(19980402);
    // 
    int i;
    for (i=0; i<N; i++) a[i] = 0;
    a[0] = 10000.0;
    a[N-1] = 10000.0;
    printf("\n");
    // Start timing
    struct timeval start;
    struct timeval end;
    gettimeofday(&start,NULL);
    // printf("start.tv_sec:%d\n",start.tv_sec);
    // printf("start.tv_usec:%d\n",start.tv_usec);

    printf("Start benchmark stencil: size=%d step=%d\n", N, tmax);
#ifdef __OPT
    stencil_opt(a, N, tmax);
#else 
    stencil(a, N, tmax);
#endif

    gettimeofday(&end,NULL);
    // printf("end.tv_sec:%d\n",end.tv_sec);
    // printf("end.tv_usec:%d\n",end.tv_usec);
    printf("Total use time: %lf\n", (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000);
    // Check answer
#ifdef __SAVE
    fp = fopen("std_ans.txt", "w");
    for (i=0; i<N; i++)
        fprintf(fp, "%lf ", a[i]);
    fprintf(fp, "\n");
#endif
    free(a);
    return 0;
}