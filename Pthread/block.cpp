#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <time.h>
#include <sys/time.h>

using namespace std;

using tensor = vector <vector<long int>>;

int block_len=2;    // length of inner blocks
long int size_mat =8;
tensor _MAT;
const int NUM_THREADS=8;
int next_pos=NUM_THREADS;
pthread_mutex_t lock;

void CreateMatrix(){
    long int val=1;
    vector<long int> num_;

      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                num_.push_back(val);
                val++;
         }_MAT.push_back(num_);
           num_.clear();
                
       }
}

void PrintMatrix(){
      for (int i=0;i<size_mat;i++){
         for (int j=0;j<size_mat;j++){
                cout << _MAT[i][j]<<"  ";
             }cout <<endl;
          }
}

tensor getSubMatrix(int A,int B){   // getting 2x2 blocks based on co-ordinates A and B
    tensor sub(block_len,vector<long int>(block_len));
   for (int i=0;i<block_len;i++){
      for (int j=0;j<block_len;j++){
            sub[i][j] = _MAT[A+i][B+j];
      }
   }
   
    return sub;
}

void setSubMatrix(int A,int B, tensor block){   // replacing 2x2 blocks based on co-ordinates A and B
   for (int i=0;i<block_len;i++){
      for (int j=0;j<block_len;j++){
             _MAT[A+i][B+j]= block[i][j];
      }
   }


}
void swap(long int& a,long int& b){
    long int tmp;
    tmp=a;
    a=b;
    b=tmp;
}

void transpose(tensor& arr){        // normal transpose algorithm
    int size;    
    size = arr.size();
    for (int i=0;i<size; i++) {
            for (int j=i+1;j<size; j++) {
                   swap(arr[i][j],arr[j][i]);
            }
    }
}

void* transpose_blocks(void* ind_){        // transposing inside of all blocks in matrix (part 1)
    
    long local;
    local = (long) ind_;

    int pos;
    int posY=0;
    int tmp = size_mat/block_len;
    int x;
    tensor block;

    while (1){
        pos = (block_len*local)%size_mat;
        x = local/tmp;
        posY = x*block_len;
        
        if (posY>=size_mat){ break;}

        block = getSubMatrix(posY,pos);
        transpose(block);
        setSubMatrix(posY,pos,block);

        pthread_mutex_lock(&lock);
            local= next_pos;
            next_pos++; 
       	pthread_mutex_unlock(&lock);
    
    }

}

void* shuffle(void* ind_){             // transposing actual blocks (part 2)
      for (int i=0;i<size_mat;i+=block_len){
        for (int j=i;j<size_mat;j+=block_len){
            tensor block = getSubMatrix(i,j);
            tensor block2 = getSubMatrix(j,i);
            setSubMatrix(i,j,block2);
            setSubMatrix(j,i,block);
        }
      }
}

int main(){
    
    CreateMatrix();
    pthread_t threads[NUM_THREADS];
    int index[NUM_THREADS];

    for (int i=0; i<NUM_THREADS;i++){
        index[i]=i;
        pthread_create(&threads[i],NULL,transpose_blocks,(void*)index[i]);
    }
    
    for (int i=0; i<NUM_THREADS;i++){
        pthread_join(threads[i],NULL);      // waiting for all blocks to be transposed
    }

     for (int i=0; i<NUM_THREADS;i++){
        index[i]=i;
        pthread_create(&threads[i],NULL,shuffle,(void*)index[i]);
    }
   // shuffle();
 //   PrintMatrix();
    
    return 0;
}
