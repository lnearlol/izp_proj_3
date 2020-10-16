#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

struct prirustek { //struktura pro pristup k pohybu
  int dr;
  int ds;
};

enum smery {VYCHOD, SEVER, ZAPAD, JIH, POCET_SMERU}; // 4 mozne smeru pohybu
struct prirustek d[POCET_SMERU] = { // presun po mape v zalezitosti od smeru
  {0, +1}, // VYCHOD 0
  {-1, 0}, // SEVER 1
  {0, -1}, // ZAPAD 2
  {+1, 0}, // JIH 3
};

int map_ctor(Map *m,FILE *file_ptr)
{
   int LEN = 500;
   char str[LEN]; // promenna pro nacteni z souboru a porovnani a spocitani poli v mape


   fscanf(file_ptr, "%d %d", &m->rows, &m->cols); // bereme velikost mapy
   m->cells = malloc(m->rows * m->cols * sizeof(unsigned char)); 
 
  int counter = 0; //pozice v m->cells
  for(int i = 0; fgets(str, LEN, file_ptr) != NULL; i++)  //udelej fgets pres cely file
  {
    for(int j = 0; str[j] != '\0'; j++)     //projdi cely str a vypisuj kazdy str[j] >= 0 a str[j] <= 7
    {
         
      if(str[j] > '7' ||  (str[j] != ' ' && str[j] < '0' && str[j] != '\n' ))
      {return 1; 
         } else 
      if(str[j] >= '0' && str[j] <= '7' && !(str[j+1] == ' ' || str[j+1] == '\n'))
        {return 1;} else 
      if(str[j] >= '0' && str[j] <= '7')
       {
         m->cells[counter] = str[j]; // kdyz elemen retezce splnuje vsechny podminky, zapis ho do mapy
         counter++; //cislo elementu v m->cells
       }
    }
    if((i > 0 && counter != (i) * m->cols)|| counter > m->rows * m->cols) //kontrola rozmernosti, kdyz cislo elementu od 0 do 7 != rows*cols, vypisujeme chybu
    {return 1;}
  }
  return 0;
}

bool isborder(Map *map, int r, int c, int border)
{
   bool right, left, top_bottom;
   left = (map->cells[c + r * map->cols] - '0') & 1; // udelame z char od 0-7 int 0-7 a kontrolujeme jestli ma spolecni bit z hranici
   right = (map->cells[c + r * map->cols] - '0') & 2;
   top_bottom = (map->cells[c + r * map->cols] - '0') & 4;

   switch(border) //vypisujeme hranice podle 'border'
    {
      case VYCHOD:
        return right;
      case ZAPAD:
        return left;
      case SEVER: case JIH:
        return top_bottom;
    }
return 0;
}

int right_left_border_test(Map *m) // funkce kontroly leve a prave hranic (jestli elementy mohou stat vedle sebe)
{
  bool c_left, c_right;
  for(int row = 0; row < m->rows; row++)
    {
      for(int col = 0; col < m->cols-1; col++) // funguje do predposledniho sloupce protoze nemuze srovnavat pravou hranice s nicim
        {
           c_left = isborder(m, row, col, VYCHOD); // m == &*m 
           c_right = isborder(m, row, col+1, ZAPAD); // kontrlo
           if(c_left != c_right) 
           {
            return 1;
           }
           
        }
    }
  return 0;
}

int top_bottom_test(Map *m)  // funkce kontroly horni a dolni hranic (jestli element [(rows + cols) % 2 == 1] ma spolecne hranice s elementem pod sebou) 
{
  bool c_top_bottom1, c_top_bottom2;
  for(int row = 0; row < m->rows-1; row++) // ciklus funguje do predposlednihi radku protoze nemuzeme porovnavat dolni hranice s nicim
    {
      for(int col = 0; col < m->cols; col++)
        { if((row+col) % 2 == 1)
            {
              c_top_bottom1 = isborder(m, row, col, JIH); // kontrola jestli element ma dolni hranicu
              c_top_bottom2 = isborder(m, row+1, col, SEVER);
              if(c_top_bottom1 != c_top_bottom2)
              {
                return 1;
              }
            }
        }
    }
return 0;
}

int start_border(Map *map, int r, int c, int leftright)
{
  int smer = 0;
  if(leftright == 1) //prava ruka
    {
      if((r+c) % 2 == 0) //pravidla pro sude r+c
      {
        if(r == 0 && isborder(map, r, c, SEVER) == false)
          {smer = ZAPAD;}
        else if(c == 0 && isborder(map, r, c, ZAPAD) == false)
          {smer = VYCHOD;}
        else if (c == map->cols-1 && isborder(map, r, c, VYCHOD) == false) 
          {smer = SEVER;} else {printf("mate spatne zadanu startovou lokace\n"); return 9;}
      }
      if((r+c) % 2 == 1) //pravidla pro liche r+c
      {
        if(r == map->rows-1 && isborder(map, r, c, JIH) == false)
          {smer = VYCHOD;} 
        else if(c == 0 && isborder(map, r, c, ZAPAD) == false)
          {smer = JIH;}
        else if (c == map->cols-1 && isborder(map, r, c, VYCHOD) == false) 
          {smer = ZAPAD;} else {printf("mate spatne zadanu startovou lokace\n"); return 9;}
      }
    }
  if(leftright == 2) //leva ruka
    {
      if((r+c)%2 == 0) //pravidla pro sude r+c
      {  
        if(r == 0 && isborder(map, r, c, SEVER) == false)
         {smer = VYCHOD;}
        else if(c == 0 && isborder(map, r, c, ZAPAD) == false)
          {smer = SEVER;}
        else if (c == map->cols-1 && isborder(map, r, c, VYCHOD) == false) 
          {smer = ZAPAD;} else {printf("mate spatne zadanu startovou lokace\n"); return 9;}
      }
      if((r+c)%2 == 1) //pravidla pro liche r+c
      {  
        if(r == map->rows-1 && isborder(map, r, c, JIH) == false)
         {smer = ZAPAD;} 
        else if(c == 0 && isborder(map, r, c, ZAPAD) == false)
         {smer = VYCHOD;}
        else if (c == map->cols-1 && isborder(map, r, c, VYCHOD) == false) 
         {smer = JIH;} else {printf("mate spatne zadanu startovou lokace\n"); return 9;}

      }
    }
  return smer;
}

int smer_change(Map *m, int smer, int row, int col, int leftright)
{

  if(leftright == 1) // pravidla pro pravou ruku
  {
   while((row >=0 && row <= m->rows - 1) && (col >= 0 && col <= m->cols - 1)) //kontrola jestli element je v hranicich [cells]
   {
    while(isborder(m, row, col, smer) == true)
      { if((row+col) % 2 == 0 && smer != ZAPAD){
          smer = (smer + 1) % POCET_SMERU;
        } else if ((row+col) % 2 == 0 && smer == ZAPAD){
          smer = (smer + 2) % POCET_SMERU;
        } else if((row+col) % 2 == 1 && smer != VYCHOD){
          smer = (smer + 1) % POCET_SMERU;
        } else if ((row+col) % 2 == 1 && smer == VYCHOD){
          smer = (smer + 2) % POCET_SMERU;
        }
      }
    printf("%d,%d\n", row+1, col+1);
    row = row + d[smer].dr;
    col = col + d[smer].ds;

       if((row+col) % 2 == 0 && smer != VYCHOD){
          smer = (smer + 3) % POCET_SMERU;
        } else if ((row+col) % 2 == 0 && smer == VYCHOD){
        } else if((row+col) % 2 == 1 && smer != ZAPAD){
          smer = (smer + 3) % POCET_SMERU;
        } else if ((row+col) % 2 == 1 && smer == ZAPAD){
        }
   }
  } else if(leftright == 2) //pravidla pro levou ruku
  {
   while((row >=0 && row <= m->rows - 1) && (col >= 0 && col <= m->cols - 1)) //kontrola jestli element je v hranicich [cells]
   {
    while(isborder(m, row, col, smer) == true)
      { if((row+col) % 2 == 0 && smer != VYCHOD){
          smer = (smer + 3) % POCET_SMERU;
        } else if ((row+col) % 2 == 0 && smer == VYCHOD){
          smer = (smer + 2) % POCET_SMERU;
        } else if((row+col) % 2 == 1 && smer != ZAPAD){
          smer = (smer + 3) % POCET_SMERU;
        } else if ((row+col) % 2 == 1 && smer == ZAPAD){
          smer = (smer + 2) % POCET_SMERU;
        }
      }
    printf("%d,%d\n", row+1, col+1);
    row = row + d[smer].dr;
    col = col + d[smer].ds;

       if((row+col) % 2 == 0 && smer != ZAPAD){
          smer = (smer + 1) % POCET_SMERU;
        } else if ((row+col) % 2 == 0 && smer == ZAPAD){
        } else if((row+col) % 2 == 1 && smer != VYCHOD){
          smer = (smer + 1) % POCET_SMERU;
        } else if ((row+col) % 2 == 1 && smer == VYCHOD){
        }
   }
  }
  return smer;
}

int args_test(int argc, char *argv[], int *leftright, int *row, int *col) 
{
   
  if(argc != 2 && argc != 3 && argc != 5){ // kdyz mame jiny pocet argumentu nez bychom potrebovali podle zadani
          fprintf(stderr, "NESPRAVNE MNOZSTVI ARGUMENTU; !2 && !3 && !5\n%d\n", argc);
    return 0;
  } else if (strcmp(argv[1], "--help") == 0){ // vypis pro help
    printf("1. ./proj3 --test bludiste.txt pro testovani vaseho bludiste\n");
    printf("2. ./proj3 --rpath R C bludiste.txt pro providlo prave ruky\n");
    printf("3. ./proj3 --lpath R C bludiste.txt pro pravidlo leve ruky\n");
    return 1;
  } else if ((strcmp(argv[1], "--rpath") == 0 || strcmp(argv[1], "--lpath") == 0) &&   //podminka na --rpath
                  atoi(argv[2]) >= 0 && atoi(argv[3]) >= 0 &&
                  strcmp(strstr(argv[4], ".txt"), ".txt") == 0 && strcmp(argv[4], ".txt") != 0)
    {
  	    *row = atoi(argv[2])-1;
            *col = atoi(argv[3])-1;
            if(strcmp(argv[1], "--rpath") == 0)
            {*leftright = 1;} else {*leftright = 2;}
	return 4;
    } 
  else if(strcmp(argv[1], "--test") == 0 && strcmp(strstr(argv[2], ".txt"), ".txt") == 0 && strcmp(argv[2], ".txt") != 0) //kontrolujeme na --test
  {
		printf("%s\n", strstr(argv[2], ".txt"));
    return 2;
  } else {return 0;}
  return 0;
}

void map_dtor(Map *m)
{
	free(m->cells); // uvolneni pameti
}

int main(int argc, char *argv[])
{
FILE *file_ptr;

  int leftright = 0; //promena po vyber pravidla ruky
  int r = -1; // zacatecne cislo radku
  int c = -1; // zacatecne cilo sloupce
  int valid = 0; // kontrola mapy
  int smer = -1; // vyber zacatecneho smeru

  int args =  args_test(argc, argv, &leftright, &r, &c); // funkce kontroly argumentu

  if(args == 2 || args == 4) //podminka na --test nebo --rpath R C soubor.txt
   {
  
    file_ptr = fopen(argv[argc - 1], "r"); //otevreni file jen pro cteni
   
    Map m;
    valid = valid + map_ctor(&m, file_ptr); 
    fclose(file_ptr); // zavirani file
    if(args == 2) //podminka na --test
    {
      valid = valid +  right_left_border_test(&m); // kontrola prave/leve hranice
      valid = valid +  top_bottom_test(&m); //kontrola horni/dolni hranice
    }
    if(args == 4 && valid == 0) // prodminka --rpath/--lpath
    {
      smer = start_border(&m, r, c, leftright); // funkce na kontrolu spravnych zacatecnych koordinat a vydani pocatecneho smera podle podminek
      if(smer != 9){ //konrola na vhodne zacatecni koordinaty
        smer_change(&m, smer, r, c, leftright);} // funkce pohybu
    }

    if(valid == 0 && args == 2 && smer != 9) // konrola vsech argumentu validnosti
    {printf("Valid\n");}
    if (valid != 0 || args == 0 || smer == 9){fprintf(stderr, "Invalid\n");} // kontrola vsech argumentu nevalidnosti
   
	map_dtor(&m); // free
   }
  
  return 1;
}