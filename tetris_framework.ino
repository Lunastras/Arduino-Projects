
#include "LedControl.h"

LedControl lc=LedControl(12,10,11,2);  // Pins: DIN,CLK,CS, # of Display connected


int bottom=15;
int top=0;

int time2wait=2000;
int secc=0;
int tet[20][8];
int tetcop[20][8];
int flagblk;
 int right=8;
  int left=9;
  int spd=13;
//make the matrix null



          //geometrical shapes

          void square()
            {
            tet[-1][5]=tet[-1][6]=tet[0][5]=tet[0][6]=2;
            }

          void line()
            {
            tet[0][2]=tet[0][3]=tet[0][4]=tet[0][5]=2;
            }

          void trishp()
            {
            tet[-1][2]=tet[-1][3]=tet[-1][4]=tet[0][3]=2;
            }




  void zeroing()
    {
    int j=0;

    for(int i=-5;i<=bottom;i++)
      for(j=0;j<8;j++)
        tet[i][j]=0;
    }

  void cop()
    {
    int i,j;
    

    for(i=0;i<=15;i++)
      for(j=0;j<8;j++)
        {
        tetcop[i][j]=tet[i][j];
        }
     ;
    }
  
  void copback()
    {
    int j=0;

    for(int i=-5;i<=bottom;i++)
      for(j=0;j<8;j++)
        tet[i][j]=tetcop[i][j];
    }



    //movement 

      void goRight()
        {
        int j,flagwall=1,flagblk=1;
        for(int i=-5;i<=bottom;i++)
          if(tet[i][7]==2)
            {
            flagwall=0;
            break;
            }

          if(flagwall)
            {
              cop();
              for(j=6;j>=0;j--)
                      {
                      for(int i=-5;i<=bottom;i++)
                          {
                              if(tet[i][j]==2 && tet[i][j+1]==0)
                                {
                                tet[i][j+1]=2;
                                tet[i][j]=0;
                                }
                                else 
                                if(tet[i][j]==2 && tet[i][j+1]==1)
                                  {
                                  flagblk=0;
                                  copback();
                                  break;
                                  }
                          }
                 if(!flagblk)break;
                }
           
            }

        }

        void goLeft()
        {
        int j,flagwall=1,flagblk=1;
          for(int i=-5;i<=bottom;i++)
            if(tet[i][0]==2)
              {
              flagwall=0;
              break;
            }

          if(flagwall)
            {
              cop();
              for(j=1;j<8;j++)
                  {
                    for(int i=-5;i<=bottom;i++)
                         {
                            if(tet[i][j]==2 && tet[i][j-1]==0)
                                  {
                                  tet[i][j-1]=2;
                                  tet[i][j]=0;
                                  }
                                else
                                if(tet[i][j]==2 && tet[i][j-1]==1)
                                      {
                                      flagblk=0;
                                      copback();
                                      break;
                                      }
                        }
                   if(!flagblk)break;
                  }
       
            }

        }


  void create()
    {
    int pic=round(random(-0.49,2.49));

    switch(pic)
      {
      case 0:
        square(); break;

      case 1:
        line(); break;

      case 2:
        trishp(); break;


      }

    }


  void display()
    {
    int e=0,i=0,j=0,adj;

    for(e=0;e<2;e++)
      {
      if(e==0)adj=0;
          else adj=8;

        for(i=0;i<8;i++)
          for(j=0;j<8;j++)
            {
            if(tet[i][j])
              lc.setLed(e,i+adj,j,true);
            }
      }
          
    }

void setup() {
 
 pinMode(right,INPUT);
 pinMode(left,INPUT);
 pinMode(spd,INPUT);
 Serial.begin(9600);

 zeroing();
 create();
 display();

}


void rem2s()
  {
  int i,j;

    for(i=top;i<=bottom;i++)
      for(j=0;j<8;j++)
        if(tet[i][j]==2)
            tet[i][j]=1;
  }



void console()
  {
   Serial.print('\n');
   Serial.print('\n');
   Serial.print('\n');
   Serial.print('\n');
   Serial.print("*********");
   Serial.print('\n');
  int i,j;
  for(i=top;i<=bottom;i++)
    {
    for(j=0;j<8;j++)
      {
      Serial.print(tet[i][j]);
      Serial.print(" ");
      }
      Serial.print('\n');
    }
   

  }



void loop() {

 flagblk=1;
 int i,j;



 cop();

 serial.print
 if(millis()-secc > time2wait)
      {
      for(i=(bottom-1);i>=(-5);i--)
          {
         for(j=0;j<8;j++)
            {
              if(tet[i][j]==2 && tet[i+1][j]==0)
                {
                  tet[i+1][j]=2;
                  tet[i][j]=0;
                }
                else
                if(tet[i][j]==2 && tet[i+1][j]==1)
                  {
                    flagblk=0;
                    copback();
                    break;
                  }
           
            }

          if(!flagblk)break;
          }


      console();
      
      secc=millis();
      }
    
  
  //when a block has nowhere else to go


    if(!flagblk)
      {
      rem2s();
      create();
      }
      else 

      for(i=0;i<8;i++)
        if(tet[bottom][i]==2)
          {
          rem2s();
          create();
          }

  //button controls

    if(digitalRead(right) == HIGH)
      goRight();

    if(digitalRead(left) == HIGH)
      goLeft();

    if(digitalRead(spd) == HIGH)
      time2wait=1000;
      else time2wait=2000;





 display();
 
}
