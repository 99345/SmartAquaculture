#ifndef SLAVE_H
#define SLAVE_H

typedef struct
{
	float tem;
	float level;
	float tds;
	float dirty;
	float ph;
}Sensor;

Sensor slave_control(void);

#endif
