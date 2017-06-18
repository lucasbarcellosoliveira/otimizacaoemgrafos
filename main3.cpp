#include <iostream>
#include "glib.hpp"
#include <algorithm>
#include <ctime>
#include <map>
#include <cmath>
#include <pthread.h>
using namespace std;

#define POPULATION 1000
#define GENERATIONS 100
#define RENEW 250
#define CROSSOVERCHANCE 0.75
#define MAXIMUM_N 100

multimap<int,int*> sol;
ALGraph g("grafo9.txt");
const int N=g.getN();
int children[GENERATIONS][RENEW][MAXIMUM_N];

void *renewing(void* id){
    long t_id=(long) id;
    int i, mut1, mut2, co1, co2, bandwidth, gen=(int)ceil(t_id/100.0);
    t_id-=gen*100;
    multimap<int,int*>::iterator it=sol.begin();
    int N=sizeof(children[0][0])/sizeof(*children[0][0]);
    for (i=0;i<sol.size();i++){
        if (i==t_id*10)
            break;
	it++;
    }
    for (int j=i;j<(t_id+1)*10;j++){
            for (int k=0;k<N;k++)
                children[gen][j][k]=(*it).second[k];
            if (rand()%100<CROSSOVERCHANCE*100){ //Crossover
               co1=rand()%N; //select first gene
               while (true){co2=rand()%N; if(co2!=co1) break;} //select second gene different from first one
               if (co1>co2) {swap(co1,co2);} //assures interval is described in ascending order
               for (int k=co1;k<=co2;k++)
                   children[gen][j][k]=(*(sol.begin())).second[k]; //inherits from best solutions
               for (int k=0;k<N;k++)
                   if (k<co1 || k>co2)
                      for (int w=0;w<N;w++)
                          if (find(&children[gen][j][0], &children[gen][j][N], (*it).second[w])==&children[i][j][N]){
                             children[gen][j][k]=(*it).second[w]; //inherits missing genes from other parent
                             break;
                          }
            }
            //Mutation
            mut1=rand()%N; //select first gene
            while (true){mut2=rand()%N; if(mut2!=mut1) break;} //select second gene different from first one
            swap(children[gen][j][mut1],children[gen][j][mut2]);
            bandwidth=-1;
            for (int k=0;k<N;k++) //Computes bandwidth for each children
               for (Element* w=g.getneighbors(k+1) ; w ; w=w->next)
                   if (abs(children[gen][j][k]-children[gen][j][w->v-1])>bandwidth) //computes larger bandwidth
                      bandwidth=abs(children[gen][j][k]-children[gen][j][w->v-1]);
            sol.insert(pair<int,int*>(bandwidth,children[gen][j])); //add children to multimap
            ++it; //goes to next best solution
    }
}

int main() {
    cout << "Press any key to load graph.\n";
    getchar(); //waits for a key to be pressed
    cout << "\n\nGraph loaded!" << endl << "Press any key to continue.\n\n\n";
    getchar(); //waits for a key to be pressed
    double duration;
    long int finish;
    long int start;
    start = clock();
    
    
    //Initializes variables
    int N=g.getN();
    int opt[N];
    int aux[POPULATION][N]; //used in swaps
    int bandwidth=-1; //bandwidth for a solution
    multimap<int,int*>::iterator it;
    int mut1, mut2, co1, co2;
    srand(time(NULL));
    pthread_t threads[RENEW/10];
    void* state; //will be used for joining threads
    
    for (int i=0;i<POPULATION;i++){
        for (int j=0;j<g.getN();j++)
            aux[i][j]=j+1;
    }
    
    //Computes first population
    for (int i=0;i<POPULATION;i++){
       random_shuffle(&aux[i][0],&aux[i][N]);
       bandwidth=-1;
       for (int w=0;w<N;w++) //Computes bandwidth for whole graph
           for (Element* j=g.getneighbors(w+1) ; j ; j=j->next)
               if (abs(aux[i][w]-aux[i][j->v-1])>bandwidth) //computes larger bandwidth
                  bandwidth=abs(aux[i][w]-aux[i][j->v-1]); 
       sol.insert(pair<int,int*>(bandwidth,aux[i]));
    }
    
    //Computes inferior boundary
    int largestDegree=-1;
    for (int i=0;i<N;i++){
        if(g.getDegree(N+1)>largestDegree)
            largestDegree=g.getDegree(N+1);
    }
    int inferior=(int)ceil(largestDegree/2.0);
    
    for (int i=0;i<GENERATIONS;i++){
        it=sol.end();
        for (int j=0;j<RENEW;j++)
            it--;
        sol.erase(it,sol.end()); //remove worst solutions
	for (int t=0;t<RENEW/10;t++) //creating threads
	    pthread_create(&threads[i],NULL,renewing,(void*)t+i*100);
	for (int t=0;t<RENEW/10;t++) //joining threads
	    pthread_join(threads[t],&state);
        if ((*(sol.begin())).first==inferior) //checks if iinferior boundary has been reached
           break;
    }
    
    
    finish = clock();
    duration = (finish - start) / (double) CLOCKS_PER_SEC;
    
    
    //for (multimap<int,int*>::iterator it=sol.begin(); it!=sol.end(); ++it){
    for (int i=0;i<N;i++)
        cout<<(*(sol.begin())).second[i]<<" "; //shows best solution
    cout<<endl;//}
    cout<<"Result = "<<(*(sol.begin())).first<<endl; //picks best solution's bandwidth
    
    
    cout << "Duration: " << duration << endl << endl;
    cout << "Done!\n" << endl << "Press any key to exit.";
    getchar(); //waits for a key to be pressed
}
