#include <stdio.h>
#include <stdlib.h>
#include<string.h>
/*
bin file structure is as follow :
width height nOw nOp [data_checksum] (w1,h1) (w2,h2) ... (wn,hn) A B C D
in which 
width 
height
nOw number of walls
nOp numbers of players
(w1,h1) (w2,h2) ... (wn,hn) are positions of each wall which is describe as a vector/line contain of 2 points (Wk,Hk)
A B C D : the position of players : A(Aw,Ah) B(Bw,Bh) ,..
all coordinates of points are 32 bits integer
*/
// #load game

typedef struct Points {
   unsigned int x;
   unsigned int y;
} Point_t;

typedef struct Pair {
   Point_t A;
   Point_t B;
} Pair_t;
typedef struct Lines {
   Point_t A;
   Point_t B;
} Line_t;
typedef struct game_data_header {
   unsigned int width;
   unsigned int height;
   unsigned int nOw;
   unsigned int nOp;
   unsigned int checksum;
} game_data_head_t;
typedef struct game_data {
   unsigned int width;
   unsigned int height;
   unsigned int nOw;
   unsigned int nOp;
   unsigned int checksum;
   Line_t * walls;
   Point_t * players;
} game_data_t;

// typedef struct game_dat {
//    file_header_t header;
//    void * data;
// } game_dat_t;

/*
This function to load saved data to buffers

*/
int load_game(game_data_t * buf,double len,char * filename){
   FILE *ptr;
   ptr = fopen(filename,"rb");
   //first read header
   // fread(&buf->width,sizeof(buf->width),1,ptr);
   // fread(&buf->height,sizeof(buf->height),1,ptr);
   // fread(&buf->nOp,sizeof(buf->nOp),1,ptr);
   // fread(&buf->nOw,sizeof(buf->nOw),1,ptr);
   // fread(&buf->checksum,sizeof(buf->checksum),1,ptr);
   fread(buf,sizeof(game_data_head_t),1,ptr);

   // caluculate size of data
   unsigned int dat_len =sizeof(Line_t)* buf->nOw + sizeof(Point_t)* buf->nOp; // sizeof(game_data_head_t) + 
   fread(buf->players,sizeof(Point_t)* buf->nOp,1,ptr);
   fread(buf->walls,sizeof(Line_t)* buf->nOw,1,ptr);
   printf("width =%u\n h=%u\n nop=%u\n nOw=%u\n\n",buf->width,buf->height,buf->nOp,
   buf->nOw);

   fclose(ptr);
   return 0;
}
// #write a chunk of data to binary file with option
int save_game(void* buf,double len,char * filename,char* save_option){
   FILE *write_ptr;
   write_ptr = fopen(filename,save_option);  // w for write, b for binary
   fflush(write_ptr);
   fwrite(buf,len,1,write_ptr); // write 10 bytes from our buffer
   fclose(write_ptr);
   return 0;
}
int checksum(void * data){
   return 0;
}
int line_of_sight(Point_t A, Point_t B, Point_t C, Point_t D){
   //  #the formula of LoS is
   printf("A-B x1=%u\n y1=%u\n x2=%u\n y2=%u\n\n",A.x,
                                                      A.y,
                                                      B.x,                                                      
                                                      B.y);
    printf("C-D x1=%u\n y1=%u\n x2=%u\n y2=%u\n\n",C.x,
                                                      C.y,
                                                      D.x,                                                      
                                                      D.y);                                                      
    float n=(float)((A.x-B.x)*(C.y-D.y)-(A.y-B.y)*(C.x-D.x));
    printf("n=%f \n",n);

    if (n == 0){
        return 0;
    }
    else{
        float m=(float)( (A.x-C.x)*(C.y-D.y)-(A.y-C.y)*(C.x-D.x));
         printf("m=%f \n",m);

        float t=m/n;
        float r=(float)((B.x-A.x)*(A.y-C.y)-(B.y-A.y)*(A.x-C.x));
        printf("r=%f \n",r);

      //   #    n=(A.x-B.x)*(C.y-D.y)-(A.y-B.y)*(C.x-D.x)
        float u=r/n;
        printf("u=%f , t=%f\n",u,t);
        if(t >= 0 && t<=1 && u >=0 && u <= 1){
            return 1;
        }
        else{
           // the sight is block 
            return 0;
        }
    }
}            

int add_wall(Line_t wall,game_data_t * p_data,char * file_name){
   //reload data and reallocate resource, maybe using link list will save memory
   // but this is out of this scope
   p_data->nOw +=1;
   Line_t * new_walls= malloc(sizeof(Line_t)* p_data->nOw);
   
   memcpy(new_walls,p_data->walls,sizeof(Line_t)* p_data->nOw);
   new_walls[p_data->nOw-1]=wall;
   // p_new_data->players = malloc(sizeof(Point_t)* p_new_data->nOp);
   // printf("prepare to load game\n");
   // load_game(p_new_data,sizeof(game_data_t),file_name);

   save_game(p_data,sizeof(game_data_head_t),file_name,"wb");
   //then write players to file
   save_game(p_data->players,sizeof(Point_t)* p_data->nOp,file_name,"ab");
   //then write walls to file
   save_game(new_walls,sizeof(Line_t)* p_data->nOw,file_name,"ab");

   return 0;
}
int remove_wall(int wall_idx,game_data_t * p_data,char * file_name){
   //reload data and reallocate resource, maybe using link list will save memory
   // but this is out of this scope
   // Line_t * newwall = malloc(sizeof(Line_t ));
   p_data->nOw -=1;
   // p_new_data->walls = malloc(sizeof(Line_t)* p_new_data->nOw);
   // p_new_data->players = malloc(sizeof(Point_t)* p_new_data->nOp);
   // printf("prepare to load game\n");
   // load_game(p_new_data,sizeof(game_data_t),file_name);

   save_game(p_data,sizeof(game_data_head_t),file_name,"wb");
   //then write players to file
   save_game(p_data->players,sizeof(Point_t)* p_data->nOp,file_name,"ab");
   //then write walls to file
   for (int c = wall_idx - 1; c < p_data->nOw - 1; c++)
         p_data->walls[c] = p_data->walls[c+1];
   save_game(p_data->walls,sizeof(Line_t)* p_data->nOw,file_name,"ab");
   
   return 0;
}
int main() {
   // printf() displays the string inside quotation
   char *file_name="game.dat";
   printf("Hello!\n");
   game_data_t * p_demo_data = malloc(sizeof(game_data_t ));
   
   printf("size of game_data_t %lu\n",sizeof(game_data_t));
   printf("init game\n");

   Line_t wall0={ .A={10,11} , .B={12,13} };
   Line_t wall1={ .A={3,35} , .B={36,70} };
   Line_t wall2={ .A={3,3} , .B={71,70} };
   Line_t wall3={ .A={3,7} , .B={12,70} };
   Line_t wall4={ .A={3,5} , .B={36,6} };
   Line_t wall_extra={ .A={31,15} , .B={36,61} };

   Point_t player0 ={.x=34, .y= 88};
   Point_t player1 ={.x=21, .y= 51};

   Point_t player2 ={.x=23, .y= 55};
   Point_t player3 ={.x=77, .y= 23};
   p_demo_data->width=800;
   p_demo_data->height=640;
   p_demo_data->nOp=4;
   p_demo_data->nOw=5;
   p_demo_data->checksum=0xffff;
   p_demo_data->walls = malloc(sizeof(Line_t)* p_demo_data->nOw);
   p_demo_data->players = malloc(sizeof(Point_t)* p_demo_data->nOp);

   p_demo_data->walls[0]=   wall0;
   p_demo_data->walls[1]=   wall1;
   p_demo_data->walls[2]=   wall2;
   p_demo_data->walls[3]=   wall3;
   p_demo_data->walls[4]=   wall4;
   p_demo_data->players[0]= player0;
   p_demo_data->players[1]= player1;
   p_demo_data->players[2]= player2;
   p_demo_data->players[3]= player3;
   printf("prepare to save game\n");
   
   unsigned int dat_len = sizeof(game_data_head_t) + sizeof(Line_t)* p_demo_data->nOw + 
    sizeof(Point_t)* p_demo_data->nOp;
   //first write header
   save_game(p_demo_data,sizeof(game_data_head_t),file_name,"wb");
   //then write players to file
   save_game(p_demo_data->players,sizeof(Point_t)* p_demo_data->nOp,file_name,"ab");
   //then write walls to file
   save_game(p_demo_data->walls,sizeof(Line_t)* p_demo_data->nOw,file_name,"ab");
   
   //reset memory
   // memset(p_demo_data->walls,0,sizeof(Line_t)* p_demo_data->nOw);
   // memset(p_demo_data->players,0,sizeof(Point_t)* p_demo_data->nOp);
   
   game_data_t* p_load_data =  malloc(sizeof(game_data_t ));
   p_load_data->width=800;
   p_load_data->height=640;
   p_load_data->nOp=4;
   p_load_data->nOw=5;
   p_load_data->checksum=0xffff;
   p_load_data->walls = malloc(sizeof(Line_t)* p_load_data->nOw);
   p_load_data->players = malloc(sizeof(Point_t)* p_load_data->nOp);
   printf("prepare to load game\n");
   load_game(p_load_data,sizeof(game_data_t),file_name);
   printf("player 1-2 x1=%u\n y1=%u\n x2=%u\n y2=%u\n\n",p_load_data->players[1].x,
                                                      p_load_data->players[1].y,
                                                      p_load_data->players[2].x,                                                      
                                                      p_load_data->players[2].y);
   // first generate pairs of players from players list
   // for  n players we have n*(n-1)/2 pairs
   unsigned int n_players= p_demo_data->nOp;
   unsigned int n_walls= p_demo_data->nOw;
   unsigned int n_pairs = n_players*(n_players-1)/2;
   
   // Pair_t * players_pair = malloc(sizeof(Pair_t) * n_pairs);
   unsigned int i,k,j;
   for(i=0;i < n_players;i++){
      for(k=i+1;k < n_players;k++){
         // players_pair[i]=
         for(j=0; j<n_walls;j++){
            unsigned int ret;
            ret = line_of_sight(p_load_data->players[i], p_load_data->players[j],
                              p_load_data->walls[j].A,p_load_data->walls[j].B);
            if(ret==0){
               printf("players %u and %u can't see each other\n",i,k);
               break;
            }
         }
      }
   }

   add_wall(wall_extra, p_load_data,file_name);
   printf("wall 2 x1=%u\n y1=%u\n x2=%u\n y2=%u\n\n",p_load_data->walls[1].A.x,
                                                      p_load_data->walls[1].A.y,
                                                      p_load_data->walls[1].B.x,                                                      
                                                      p_load_data->walls[1].B.y);
   remove_wall(5, p_load_data,file_name);
   printf("width =%u\n h=%u\n nop=%u\n nOw=%u\n\n",p_load_data->width,p_load_data->height,p_load_data->nOp,
   p_load_data->nOw);
   // p_demo_data->nOw);   
   free(p_demo_data);
   free(p_demo_data->walls);
   free(p_demo_data->players);
   free(p_load_data);
   free(p_load_data->walls);
   free(p_load_data->players);
   // free(p_demo_data);

   return 0;
}
