/* 
 * File:   simple.h
 * Author: xetk
 *
 * Created on April 10, 2013, 6:54 AM
 */
#include <libplayerc++/playerc++.h>
#ifndef SIMPLE_H
#define	SIMPLE_H

class simple
{
public:
    static void turnangle(int angle);
    static void turntoangle(int angle);
    static void align();
    static void move(int distance);
};

#endif	/* SIMPLE_H */
