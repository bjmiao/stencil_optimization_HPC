#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
// 2D 5-point stencil

#define image_2d(i,j) ((i)*M+(j))

typedef struct {int x0, dx0, x1, dx1;} C;

void stencil(double a[], int N, int M, int tmax)
{
    int i,j;
    while (tmax--){
        for (i=1; i<N-1; i++)
            for (j=1; j<M-1; j++){
                a[image_2d(i,j)] = (a[image_2d(i+1,j)] + a[image_2d(i-1,j)] + a[image_2d(i,j-1)] + a[image_2d(i,j+1)]) / 4;
            }
    }
    return ;
}

void basecase(int queue[], int t0, C c[2])
{
    static int i=0;


}

void walk(int queue[], int t0, int t1, C c[2])
{
    int dt = t1-t0;
    if (dt == 1){
        basecase(queue, t0, c);
    } else if (dt > 1){
        C *p;
        for (p=c; p<c+2; ++p){
            int x0 = p->x0, x1=p->x1, dx0=p->dx0, dx1=p->dx1;
            if (2* (x1-x0) + (dx1-dx0)*dt >= 4 * dt){
                // cut space dimension *p
                C save = *p;
                int xm = (2*(x0+x1) + (2 + dx0 + dx1) * dt)/4;
                *p = (C){x0, dx0, xm, -1}; walk(queue, t0, t1, c);
                *p = (C){xm, -1, x1, dx1}; walk(queue, t0, t1, c);
                *p = save;
                return;
            }
        }
    }
    {
        // cut time
        int s = dt / 2;
        C newc[2];
        int i;
        walk(queue,t0, t0+s, c);
        for (i=0; i<2; i++){
            newc[i] = (C){c[i].x0 + c[i].dx0*s, c[i].dx0,
                          c[i].x1 + c[i].dx1*s, c[i].dx1};
        }
        walk(queue,t0+s, t1, newc);    
    }

}

void stencil_opt(double a[], int N, int M, int tmax)
{
    int i,j;
    int t_period = tmax / 10;
    C c[2];
    c[0].x0 = c[1].x0 = 0;
    c[0].x1 = N; c[1].x1 = M;
    c[0].dx0 = c[0].dx1 = c[1].dx0 = c[1].dx1 = 0;
    int *queue = (int*) malloc(N*M*t_period);
    walk(queue, 0, tmax, c);

    int ntime;
    for(ntime=0; ntime<10; ntime++)
    for (i=0; i<M*N*t_period; i++){
        // int x=queue[i] / M;
        // int y=queue[i] % M;
        int x = queue[i];
        int x_n = x + M;
        int x_s = x - M;
        int x_w = x - 1;
        int x_e = x + 1;
        a[x] = (a[x_n] + a[x_s] + a[x_w] + a[x_e])/4;
    }

    free(queue);
}

int main(int argc, char* argv[])
{

    FILE *fp;
    fp = fopen("size_2d.txt","r");
    if (!fp) {printf("Input file not exist\n"); exit(1);}
    int N, M, tmax;
    fscanf(fp, "%d%d%d", &N, &M, &tmax);
    fclose(fp);
    double *a;
    a = (double*) malloc(N * M * sizeof(double));

    // srand(19980402);
    // 
    int i,j;
    for (i=0; i<N; i++) a[image_2d(i,0)] = 10000;
    for (i=0; i<N; i++) a[image_2d(i,M-1)] = -10000;
    for (i=0; i<N; i++)
        for (j=1; j<M-1; j++) a[image_2d(i,j)] = 0; 
    // a[0] = 10000.0;
    // a[N-1] = 10000.0;
    // printf("\n");

    // Start timing
    struct timeval start;
    struct timeval end;
    gettimeofday(&start,NULL);
    // printf("start.tv_sec:%d\n",start.tv_sec);
    // printf("start.tv_usec:%d\n",start.tv_usec);

    printf("Start benchmark 2d stencil: size=(%d,%d) step=%d\n", N, M, tmax);
#ifdef __OPT
    stencil_opt(a, N, M, tmax);
#else 
    stencil(a, N, M, tmax);
#endif

    gettimeofday(&end,NULL);
    // printf("end.tv_sec:%d\n",end.tv_sec);
    // printf("end.tv_usec:%d\n",end.tv_usec);
    printf("Total use time: %lf\n", (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000);
    // Check answer
#ifdef __SAVE
    fp = fopen("std_ans_2d.txt", "w");
    for (i=0; i<N; i++){
        for (j=0; j<M; j++) fprintf(fp, "%lf ", a[image_2d(i,j)]);
        fprintf(fp, "\n");
    }
#endif
    free(a);
    return 0;
}