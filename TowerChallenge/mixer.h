#pragma once

#include <string>
#include <cstdint>

typedef struct
{
	int state;                      // state of the sound ?? Playing/Stopped
	unsigned int pos;
	int loaded_sample_num;
	int id;
	int looping;
	double vol;
	int stream_type;
} CHANNEL;


typedef struct
{
	int channels;		        //<  Number of channels
	unsigned short sampleRate;	/**<  Sample rate */
	unsigned long sampleCount;	/**<  Sample count */
	unsigned long dataLength;	/**<  Data length */
	int16_t bitPerSample;	    /**<  The bit rate of the WAV */
	int state;                  //Sound loaded or sound empty
	int num;
	std::string name;
	union {
		uint8_t *u8;            /* data for 8 bit samples */
		int16_t *u16;           /* data for 16 bit samples */
		void *buffer;           /* generic data pointer to the actual wave data*/
	} data;
}SAMPLE;


void init_mixer(int rate, int fps);
void update_mixer();
void end_mixer();
int load_sample(const char *archname, const char *filename);
void sample_stop(int chanid);
void sample_start(int chanid, int samplenum, int loop);
void sample_start(int chanid, const std::string name, int loop);
void sample_set_position(int chanid, int pos);
void sample_set_volume(int chan, int volume);
void sample_set_freq(int channid, int freq);
int sample_playing(int chanid);
void sample_end(int chanid);
void sample_remove(int samplenum);
//Streaming audio functions added/tacked on.
void stream_start(int chanid, int stream);
void stream_stop(int chanid, int stream);
void stream_update(int chanid, int stream, short *data);
//Internal functions
int sample_up16(int num);
int snumlookup(int snum);
int create_sample(const std::string &name, int bits, int channels, int freq, int len);
int nameToNum(std::string name);
std::string numToName(int num);





