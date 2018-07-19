/* footsteps.c
 *
 * Original code by Tony Tavener
 * https://cranialburnout.blogspot.com/2012/08/openal-soft-demonstration-of-binaural.html
 *
 * To compile:
 *   gcc -o footsteps footsteps.c -lopenal
 *
 * Requires data "footsteps.raw", which is any signed-16bit
 * mono audio data (no header!); assumed samplerate is 44.1kHz.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  /* for usleep() */
#include <math.h>    /* for sqrtf() */
#include <time.h>    /* for time(), to seed srand() */
#include <stdbool.h>
#include <sndfile.h>
#include <signal.h>

/* OpenAL headers */
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include "effects.h"

int CheckAlError ()
{
	ALenum alErrCode = alGetError ();
	
	if ( alErrCode != AL_NO_ERROR )
	{
		const char* errStr = NULL;
		switch ( alErrCode )
		{
		case ( AL_INVALID_NAME ):      errStr = "AL_INVALID_NAME"; break;
		case ( AL_INVALID_ENUM ):      errStr = "AL_INVALID_ENUM"; break;
		case ( AL_INVALID_VALUE ):     errStr = "AL_INVALID_VALUE"; break;
		case ( AL_INVALID_OPERATION ): errStr = "AL_INVALID_OPERATION"; break;
		case ( AL_OUT_OF_MEMORY ):     errStr = "AL_OUT_OF_MEMORY"; break;
		
		default:
			errStr = "UNKNOWN";
			break;
		}
		
		fprintf ( stderr, "OpenAL error %04X: %s\n", alErrCode, errStr );
		return alErrCode;
	}

	return 0;
}

bool running = true;

void RequestClose ( int a )
{
	running = false;
}

int LoadWave ( ALuint a_buffer, const char* a_path )
{
	SF_INFO sndinfo;
	sndinfo.format = 0;
	SNDFILE* sndfile = sf_open ( a_path, SFM_READ, &sndinfo );
	if ( sndfile == NULL )
	{
		return -1;
	}
	
	void* data = malloc ( sndinfo.frames * sndinfo.channels * sizeof(uint16_t) );
	if ( data == NULL )
	{
		sf_close ( sndfile );
		return -2;
	}
	
	int read = (int)sf_read_short ( sndfile, data, sndinfo.frames );
	
	ALenum fmt = ( sndinfo.channels == 2 ) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	alBufferData ( a_buffer, fmt, data, read * sndinfo.channels  * sizeof(uint16_t), sndinfo.samplerate );
	free ( data );
	
	sf_close ( sndfile );
	return 0;
}

float Frand ()
{
	return (float)rand () / RAND_MAX;
}

float FrandRange ( float a_from, float a_to )
{
	return a_from + Frand () * ( a_to - a_from );
}

int main ( int argc, char* argv[] )
{
	if ( LoadProc () )
	{
		return -1;
	}
	
	/* current position and where to walk to... start just 1m ahead */
	float pos[2] = { 0.0f, -1.0f };
	float tgt[2] = { 0.0f, -1.0f };
	
	/* initialize OpenAL context, asking for 44.1kHz to match HRIR data */
	//ALCint contextAttr[] = { ALC_FREQUENCY, 44100, 0 };
	ALCdevice* device = alcOpenDevice ( NULL );
	if ( device == NULL )
	{
		return -1;
	}
	
	ALCcontext* context = alcCreateContext ( device, NULL );
	if ( context == NULL )
	{
		return -1;
	}
	
	alcMakeContextCurrent ( context );
	if ( CheckAlError () )
	{
		return -1;
	}
	
	// Check EFX
	if ( !alcIsExtensionPresent(alcGetContextsDevice ( alcGetCurrentContext () ), "ALC_EXT_EFX" ) || CheckAlError () )
    {
        fprintf ( stderr, "Error: EFX not supported\n" );
        return 1;
    }
	
	/* listener at origin, facing down -z (ears at 1.5m height) */
	alListener3f ( AL_POSITION, 0.0f, 1.5, 0.0f );
	alListener3f ( AL_VELOCITY, 0.0f, 0.0f, 0.0f );
	float orient[6] = { /*fwd:*/ 0.0f, 0.0f, -1.0f, /*up:*/ 0.0f, 1.0f, 0.0f };
	alListenerfv ( AL_ORIENTATION, orient );
	
	/* this will be the source of ghostly footsteps... */
	ALuint source, srcMemes;
	alGenSources ( 1, &source );
	alSourcef ( source, AL_PITCH, 1.0f );
	alSourcef ( source, AL_GAIN, 1.0f );
	alSource3f ( source, AL_POSITION, pos[0], 0.0f, pos[1] );
	alSource3f ( source, AL_VELOCITY, 0.0f, 0.0f, 0.0f );
	alSourcei ( source, AL_LOOPING, AL_TRUE );
	
	alGenSources ( 1, &srcMemes );
	alSourcef ( srcMemes, AL_PITCH, 1.0f );
	alSourcef ( srcMemes, AL_GAIN, 0.75f );
	alSource3f ( source, AL_POSITION, pos[0], 1.5f, pos[1] );
	alSource3f ( source, AL_VELOCITY, 0.0f, 0.0f, 0.0f );
	
	/* allocate an OpenAL buffer and fill it with monaural sample data */
	ALuint buffer;
	ALuint memes[16];
	alGenBuffers ( 1, &buffer );
	alGenBuffers ( 16, memes );
	LoadWave ( buffer, "dat/footsteps.wav" );
	for ( int i = 0; i < 16; ++i )
	{
		char path[128];
		snprintf ( path, 127, "dat/%i.aiff", i );
		LoadWave ( memes[i], path );
	}
	
	// what
	EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_GENERIC;
	ALuint efx = LoadEffect ( &reverb );
	ALuint efxSlot = 0;
	alGenAuxiliaryEffectSlots ( 1, &efxSlot );
	alAuxiliaryEffectSloti ( efxSlot, AL_EFFECTSLOT_EFFECT, efx );
	
	alSourcei ( source, AL_BUFFER, buffer );
	alSource3i ( source, AL_AUXILIARY_SEND_FILTER, efxSlot, 0, AL_FILTER_NULL );
	alSource3i ( srcMemes, AL_AUXILIARY_SEND_FILTER, efxSlot, 0, AL_FILTER_NULL );
	
	/* state initializations for the upcoming loop */
	srand ( (int) time (NULL) );
	float dt = 1.0f / 60.0f;
	float vel = 0.8f * dt;
	float closeEnough = 0.05;
	float loiterTime = 0.0f;
	float memeTime = 0.0f;
	
	/** BEGIN! **/
	alSourcePlay ( source );
	
	fflush ( stderr ); /* in case OpenAL reported an error earlier */
	signal ( SIGINT, RequestClose );
	
	/* loop forever... walking to random, adjacent, integer coordinates */
	while ( running )
	{
		if ( memeTime > 0.0f )
		{
			memeTime -= dt;
			if ( memeTime < 0.0f )
			{
				alSourcei ( srcMemes, AL_BUFFER, memes[rand () % 16] );
				printf ( "blah blah blah\n" );
				alSourcePlay ( srcMemes );
			}
		}
		
		if ( loiterTime > 0.0f )
		{
			loiterTime -= dt;
			if ( loiterTime < 0.0f )
			{
				loiterTime = 0.0f;
				printf ( "keep on walkin'\n" );
				alSourcePlay ( source );
			}
		}
		else
		{
			float dx = tgt[0] - pos[0];
			float dy = tgt[1] - pos[1];
			float dist = sqrtf ( dx * dx + dy * dy );
			if ( dist < closeEnough )
			{
				tgt[0] = FrandRange ( -4.0f, 4.0f );
				tgt[1] = FrandRange ( -4.0f, 4.0f );
				printf ( "Walkin' to: %.1f,%.1f,%.1f\n", tgt[0] ,0.0f ,tgt[1] );
				loiterTime = FrandRange ( -3.0f, 2.0f );
				if ( loiterTime > 0.0f )
				{
					printf ( "loitering for %.1f sec..\n", loiterTime );
					alSourcePause ( source );
				}
				
				ALint state;
				alGetSourcei ( srcMemes, AL_SOURCE_STATE, &state );
				if ( state != AL_PLAYING && Frand () > 0.5 )
				{
					memeTime = FrandRange ( 0.0f, 3.33f );
					printf ( "speek 2 me cont in %.1f sec..\n", memeTime );
				}
			}
			else
			{
				float toVel = vel / dist;
				float v[2] = { dx * toVel, dy * toVel };
				pos[0] += v[0];
				pos[1] += v[1];
				
				alSource3f ( source, AL_POSITION, pos[0], 0.0f, pos[1] );
				alSource3f ( srcMemes, AL_POSITION, pos[0], 1.5f, pos[1] );
				alSource3f ( source, AL_VELOCITY, v[0], 0.0f, v[1] );
				alSource3f ( srcMemes, AL_VELOCITY, v[0], 0.0f, v[1] );
				
			}
		}
		
		usleep ( (unsigned int)( 1e6 * dt ) );
	}
	
	/* cleanup that should be done when you have a proper exit... ;) */
	alDeleteSources ( 1, &srcMemes );
	alDeleteSources ( 1, &source );
	alDeleteBuffers ( 16, memes );
	alDeleteBuffers ( 1, &buffer );
	alcDestroyContext ( context );
	alcCloseDevice ( device );
	
	return 0;
}
