#include <iostream>
#include "glib.hpp"
#include <ctime>
#include <cmath>
using namespace std;

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
    int sol[g.getN()];
    int opt[g.getN()];
    int c[g.getN()]; //used in Heap Permutation
    int bandwidth=-1; //bandwidth for a solution
    int optbw=-1; //optimal bandwidth
    bool forAll; //true if bandwidth should be computed for all vertices
    int N=g.getN();
    int largestDegree=-1;
    int inferior;
    
    for (int i=0;i<N;i++){
        sol[i]=i+1;
        c[i]=0;
        if(g.getDegree(N+1)>largestDegree)
            largestDegree=g.getDegree(N+1);
    }
    
    inferior=(int)ceil(largestDegree/2.0);
    
    //Computes first solution for a starting point: [1,2,3,...,n]
    for (int i=0;i<N;i++){
        for (Element* j=g.getneighbors(i+1) ; j ; j=j->next)
            if (abs(sol[i]-sol[j->v-1])>bandwidth) //computes larger bandwidth
               bandwidth=abs(sol[i]-sol[j->v-1]);
        if (bandwidth<optbw || optbw==-1){ //stores smaller maximum
           optbw=bandwidth;
           for (int k=0;k<N;k++)
               opt[k]=sol[k];
        }
    }
    
    if (optbw!=inferior)
        //Heap Permutation
        for (int i=0;i<N;i++){
            forAll=true;
            if (c[i]<i){
               if (i%2==0){
                  swap(sol[0],sol[i]);
                  bandwidth=-1;
                  for (Element* j=g.getneighbors(1) ; j ; j=j->next) //'Core'
                      if (abs(sol[0]-sol[j->v-1])>bandwidth)
                         bandwidth=abs(sol[0]-sol[j->v-1]);
                  if (bandwidth>=optbw)
                     forAll=false;
               }
               else{
                  swap(sol[c[i]],sol[i]);
                  bandwidth=-1;
                  for (Element* j=g.getneighbors(c[i]+1) ; j ; j=j->next) //'Core'
                      if (abs(sol[c[i]]-sol[j->v-1])>bandwidth)
                         bandwidth=abs(sol[c[i]]-sol[j->v-1]);
                  if (bandwidth>=optbw)
                     forAll=false;
               }
               bandwidth=-1;
               for (Element* j=g.getneighbors(i+1) ; j ; j=j->next) //'Core'
                  if (abs(sol[i]-sol[j->v-1])>bandwidth)
                     bandwidth=abs(sol[i]-sol[j->v-1]);
               if (bandwidth>=optbw)
                 forAll=false;
               if (forAll){
                  bandwidth=-1;
                  for (int w=0;w<N;w++) //Computes bandwidth for whole graph, not only swapped marcations
                       for (Element* j=g.getneighbors(w+1) ; j ; j=j->next)
                           if (abs(sol[w]-sol[j->v-1])>bandwidth) //computes larger bandwidth
                              bandwidth=abs(sol[w]-sol[j->v-1]); 
                   if (bandwidth<optbw){ //stores smaller maximum
                      optbw=bandwidth;
                      for (int k=0;k<N;k++)
                          opt[k]=sol[k];
                      if(optbw==inferior)
                          break;
                   }
               }
               c[i]+=1;
               i=-1;
            }
            else
                c[i]=0;
        }
    
   
    finish = clock();
    duration = (finish - start) / (double) CLOCKS_PER_SEC;
    
    
    for (int i=0;i<N;i++)
        cout<<opt[i]<<" ";
    cout<<endl;
    cout<<"Result = "<<optbw<<endl;
    
    
    cout << "Duration: " << duration << endl << endl;
    cout << "Done!\n" << endl << "Press any key to exit.";
    getchar(); //waits for a key to be pressed
}
