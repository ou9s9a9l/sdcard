/*
 * sound.h
 *
 * Created: 2014/6/6 16:32:39
 *  Author: oyx
 */ 


#ifndef SOUND_H_
#define SOUND_H_
struct string{
	char name[11];
	char attri;
	char leave[8];
	unsigned int high;
	char leave1[4];
	unsigned int low;
	unsigned long length;
};

#define Storage 32768//8g



#endif /* SOUND_H_ */