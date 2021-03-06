#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "Mapt.h"
#include "simple.h"
#include "mapc.h"
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

static PlayerClient    robot("localhost");
static Position2dProxy pp(&robot,0);
static RangerProxy     sp(&robot,0);
static bool retmenu = false;
//main method, this is where the magic happens
int main(int argc, char *argv[])
{
    
    signal(SIGINT,simple::do_sigint);
    
	pp.SetMotorEnable(true);
        robot.Read();
        
        Mapt::start();
        
        int prex, prey, dir;
        
        
        simple::move(1);
        simple::align();
       
        
        /*simple::turnangle(90);
        simple::move(22);
        simple::turnangle(-90);
        simple::move(1);
        simple::turnangle(-90);
        simple::move(22);
        simple::turnangle(90);
        simple::move(10);
        simple::turnangle(-90);
        simple::move(1);
        simple::turnangle(-90);
        simple::move(20);
        simple::turnangle(-90);
        simple::move(20);*/

	for(;;)
	{
            int x = ((pp.GetXPos() * 100) / 60) +16, y = ((pp.GetYPos() * 100) / 60) + 16;
            if (!retmenu)
            {
		robot.Read();
                if (sp.GetRangeCount() != 0)
                {


                    if (x != prex||y != prey)
                    {
                        prex = x;
                        prey = y;
                        Mapt::sens(sp,x,y,rtod(pp.GetYaw()));
                        mapc m(x,y);
                    }
                    simple::nav();
                            
                }
            }
            else
            {
                pp.SetSpeed(0,0);
                cout << "Menu" << endl;
                cout << "0: Exit" << endl;
                cout << "1: Return to nav" << endl;
                cout << "2: Go to cord" << endl;
                cout << "3: Turn to angle " << endl;
                cout << "4: Move" << endl;
                int a;
                cin >> a;
                switch (a)
                {
                    case 0:
                        exit(0);
                        break;
                    case 1:
                        retmenu = false;
                        break;
                    case 2:
                        cout << "Current Location : " << x << " " << y << endl;
                        int px, py;
                        cout << "Enter x " << endl;
                        cin >> px;
                        cout << "Enter y " << endl;
                        cin >> py;
                        simple::gotocord(px,py);
                        break;
                    case 3:
                        cout << "Enter Angle" << endl;
                        int d;
                        cin >> d;
                        simple::turntoangle(d);
                        break;
                    case 4:
                        cout << "Enter Move" << endl;
                        int v;
                        cin >> v;
                        simple::move(v);
                        break;
                    default:
                        retmenu = false;
                        break;
                }
                    
            }
	}
        
}
//this method is to navigate around the area and make sure we dont bump into anything
void simple::nav()
{
    if (sp[3] < 0.5||sp[4] < 0.5)
    {
        if ((int)sp[7] == (int)sp[0])
        {
            turnangle(90);
        }
        else
        {
            if (sp[7] < sp[0])
            {
                turnangle(90);
            }
            else
            {
                turnangle(-90);
            }
            align();
        }
        if (sp[3] > 0.75||sp[4] > 0.75)
        {
                move(1);
        }
    }
               
    
    if (sp[0] > 4 && sp[15] > 4)
    {
        if ((int)sp[7] != (int)sp[0])
        {
            move(1);
            turnangle(90);
            move(2);
            align();
        }
    }
    pp.SetSpeed(0.5,0);
}
//this method is to build a route to a set cordernate 
void simple::gotocord(int vx, int vy)
{
	//this method is unfinished
     int ix = ((pp.GetXPos() * 100) / 60) + 16, iy = ((pp.GetYPos() * 100) / 60) + 16;
     cout << vx << " " << vy << " " << ix << " " << iy << endl;
     vector<int> l = Mapt::search(ix,iy,vx,vy);
     for (int i = 0; i < l.size();i++)
     {
         turntoangle(l[i]);
         move(1);
     }
}
//rotate the robot to than angle
void simple::turnangle(int angle)
{
    cout << "turn angle" << endl;
    int pred = (rtod(pp.GetYaw()) + 180);
    int dd = pred + angle;
    
    if (dd > 359)
        dd = dd - 360;
    
    if (dd < 0)
        dd = dd + 360;
    
    for (;;)
    {
        robot.Read();
        double turnrate = 0;
        int curd = (rtod(pp.GetYaw()) + 180);
        if (curd == dd)
            break;
        
        if (dd > curd)
            if ((dd - curd) > 10)
                turnrate = dtor(10);
            else
                turnrate = dtor(1);
        else
            if ((curd - dd) > 10)
                turnrate = dtor(-10);
            else
                turnrate = dtor(-1);
        
        pp.SetSpeed(0,turnrate);     
    }       
}
//turn the robot to a set angle
void simple::turntoangle(int angle)
{
    cout << "turn to angle" << endl;
    int pred = (rtod(pp.GetYaw()) + 180);
    int dd = angle;
    
    if (dd > 359)
        dd = dd - 360;
    if (dd < 0)
        dd = dd + 360;
    
    for (;;)
    {
            robot.Read();
            double turnrate = 0;
            int curd = (rtod(pp.GetYaw()) + 180);

            if (curd == dd)
                break;

            if (dd > curd)
                if ((dd - curd) > 10)
                    turnrate = dtor(10);
                else
                    turnrate = dtor(1);
            else
                if ((curd - dd) > 10)
                    turnrate = dtor(-10);
                else
                    turnrate = dtor(-1);

            pp.SetSpeed(0,turnrate);   
    }  
}
//the align method makes sure that the robot is a set distance away from the wall before it takes sensor readings
void simple::align()
{
    cout << "Align" << endl;
    robot.Read();
    if (sp.GetRangeCount() != 0)
    {
        int dir = 0;
        
        if (sp[7] > sp[0])
        {
            turnangle(90);
            dir = 1;
        }
        else
        {
            turnangle(-90);
            dir = -1;
        }
            
        for (;;)
        {
            robot.Read();
            
            if (sp[3] < 0.45&&sp[4] <0.45)
            {
                pp.SetSpeed(-0.1,0);
            }
            else if (sp[3] > 0.55&&sp[4] > 0.55)
            {
                pp.SetSpeed(0.1,0);
            }
            else
            {
                if (dir == -1)
                {
                    turnangle(90);
                }
                else if (dir == 1)
                {
                    turnangle(-90);
                }
                break;
            }
        }
    }
    pp.SetSpeed(0,0);
}
//movement moves the robot forward one grid refrence forward
void simple::move(int distance)
{

    cout << "move" << endl;
    int angle = rtod(pp.GetYaw()) + 180;
    
    if ((angle % 90) != 0)
        turntoangle((int)(angle / 90) * 90);
    
    int dir = angle / 90;

    cout << "Dir " << dir << endl;
    
    int prex = ((pp.GetXPos() * 100) / 60) + 16, prey = ((pp.GetYPos() * 100) / 60) + 16;
        
    rsens **map;
    Mapt::getGrid(&map);

    if (map)
    {
        map[prex][prey].read = -1;
        map[prex][prey].type = "";
    }
    
    for (;;)
    {
        robot.Read(); 
        
        if (((sp[4] + sp[3])/2) < 0.4)
            break;
        
        int x = ((pp.GetXPos() * 100) / 60) + 16, y = ((pp.GetYPos() * 100) / 60) + 16;
        
        cout << dir << " " << x << " " << y << " " << endl;
        
        if (dir == 2)
        {
            if (prex + distance == x)
            {
                break;
            }
        }
        else if (dir == 0)
        {
            if (prex - distance == x)
            {
                break;
            }
        }
        else if (dir == 1)
        {
            cout << (prey - distance) << endl;
            if (prey - distance == y)
            {
                break;
            }
        }
        else if (dir == 3)
        {
            if (prey + distance == y)
            {
                break;
            }
        }
        pp.SetSpeed(0.5, 0);
    }
    pp.SetSpeed(0,0);
}
//todo with enabling the menu
void simple::do_sigint(int dummy)
{
    cout << "Menu activated: pleas wait" << endl;
    retmenu = true;
}
