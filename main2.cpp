#include <iostream>
#include "glib.hpp"
#include <algorithm>
#include <ctime>
#include <map>
#include <cmath>
using namespace std;

#define POPULATION 1000
#define GENERATIONS 100
#define RENEW 250
#define CROSSOVERCHANCE 0.75

int main() {
    cout << "Press any key to load graph.\n";
    getchar(); //waits for a key to be pressed
    ALGraph g("grafo9.txt");	//  LOAD THE GRAPH HERE
    cout << "\n\nGraph loaded!" << endl << "Press any key to continue.\n\n\n";
    getchar(); //waits for a key to be pressed
    double duration;
    long int finish;
    long int start;
    start = clock();
    
    
    //Initializes variables
    multimap<int,int*> sol;
    int opt[g.getN()];
    int aux[POPULATION][g.getN()]; //used in swaps
    int bandwidth=-1; //bandwidth for a solution
    int N=g.getN();
    multimap<int,int*>::iterator it;
    int children[GENERATIONS][RENEW][N], mut1, mut2, co1, co2;
    srand(time(NULL));
    
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
        it=sol.begin();
        for (int j=0;j<RENEW;j++){
            for (int k=0;k<N;k++)
                children[i][j][k]=(*it).second[k];
            if (rand()%100<CROSSOVERCHANCE*100){ //Crossover
               co1=rand()%N; //select first gene
               while (true){co2=rand()%N; if(co2!=co1) break;} //select second gene different from first one
               if (co1>co2) {swap(co1,co2);} //assures interval is described in ascending order
               for (int k=co1;k<=co2;k++)
                   children[i][j][k]=(*(sol.begin())).second[k]; //inherits from best solutions
               for (int k=0;k<N;k++)
                   if (k<co1 || k>co2)
                      for (int w=0;w<N;w++)
                          if (find(&children[i][j][0], &children[i][j][N], (*it).second[w])==&children[i][j][N]){
                             children[i][j][k]=(*it).second[w]; //inherits missing genes from other parent
                             break;
                          }
            }
            //Mutation
            mut1=rand()%N; //select first gene
            while (true){mut2=rand()%N; if(mut2!=mut1) break;} //select second gene different from first one
            swap(children[i][j][mut1],children[i][j][mut2]);
            bandwidth=-1;
            for (int k=0;k<N;k++) //Computes bandwidth for each children
               for (Element* w=g.getneighbors(k+1) ; w ; w=w->next)
                   if (abs(children[i][j][k]-children[i][j][w->v-1])>bandwidth) //computes larger bandwidth
                      bandwidth=abs(children[i][j][k]-children[i][j][w->v-1]);
            sol.insert(pair<int,int*>(bandwidth,children[i][j])); //add children to multimap
            ++it; //goes to next best solution
        }
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
