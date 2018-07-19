#include "effects.h"

#include <stdio.h>
#include <assert.h>

/* Effect object functions */
LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
LPALEFFECTI alEffecti;
LPALEFFECTIV alEffectiv;
LPALEFFECTF alEffectf;
LPALEFFECTFV alEffectfv;
LPALGETEFFECTI alGetEffecti;
LPALGETEFFECTIV alGetEffectiv;
LPALGETEFFECTF alGetEffectf;
LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

int LoadProc ()
{
	/* Define a macro to help load the function pointers. */
#define LOAD_PROC(x) (x) = alGetProcAddress(#x); assert(alGetError())
	LOAD_PROC(alGenEffects);
	LOAD_PROC(alDeleteEffects);
	LOAD_PROC(alIsEffect);
	LOAD_PROC(alEffecti);
	LOAD_PROC(alEffectiv);
	LOAD_PROC(alEffectf);
	LOAD_PROC(alEffectfv);
	LOAD_PROC(alGetEffecti);
	LOAD_PROC(alGetEffectiv);
	LOAD_PROC(alGetEffectf);
	LOAD_PROC(alGetEffectfv);

	LOAD_PROC(alGenAuxiliaryEffectSlots);
	LOAD_PROC(alDeleteAuxiliaryEffectSlots);
	LOAD_PROC(alIsAuxiliaryEffectSlot);
	LOAD_PROC(alAuxiliaryEffectSloti);
	LOAD_PROC(alAuxiliaryEffectSlotiv);
	LOAD_PROC(alAuxiliaryEffectSlotf);
	LOAD_PROC(alAuxiliaryEffectSlotfv);
	LOAD_PROC(alGetAuxiliaryEffectSloti);
	LOAD_PROC(alGetAuxiliaryEffectSlotiv);
	LOAD_PROC(alGetAuxiliaryEffectSlotf);
	LOAD_PROC(alGetAuxiliaryEffectSlotfv);
#undef LOAD_PROC
	
	return 0;
}

ALuint LoadEffect(const EFXEAXREVERBPROPERTIES *reverb)
{
	ALuint effect = 0;
	ALenum err;

	/* Create the effect object and check if we can do EAX reverb. */
	alGenEffects(1, &effect);
	if(alGetEnumValue("AL_EFFECT_EAXREVERB") != 0)
	{
		printf("Using EAX Reverb\n");

		/* EAX Reverb is available. Set the EAX effect type then load the
		 * reverb properties. */
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

		alEffectf(effect, AL_EAXREVERB_DENSITY, reverb->flDensity);
		alEffectf(effect, AL_EAXREVERB_DIFFUSION, reverb->flDiffusion);
		alEffectf(effect, AL_EAXREVERB_GAIN, reverb->flGain);
		alEffectf(effect, AL_EAXREVERB_GAINHF, reverb->flGainHF);
		alEffectf(effect, AL_EAXREVERB_GAINLF, reverb->flGainLF);
		alEffectf(effect, AL_EAXREVERB_DECAY_TIME, reverb->flDecayTime);
		alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
		alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, reverb->flDecayLFRatio);
		alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
		alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
		alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, reverb->flReflectionsPan);
		alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
		alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
		alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, reverb->flLateReverbPan);
		alEffectf(effect, AL_EAXREVERB_ECHO_TIME, reverb->flEchoTime);
		alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, reverb->flEchoDepth);
		alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, reverb->flModulationTime);
		alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, reverb->flModulationDepth);
		alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
		alEffectf(effect, AL_EAXREVERB_HFREFERENCE, reverb->flHFReference);
		alEffectf(effect, AL_EAXREVERB_LFREFERENCE, reverb->flLFReference);
		alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
		alEffecti(effect, AL_EAXREVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
	}
	else
	{
		printf("Using Standard Reverb\n");

		/* No EAX Reverb. Set the standard reverb effect type then load the
		 * available reverb properties. */
		alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

		alEffectf(effect, AL_REVERB_DENSITY, reverb->flDensity);
		alEffectf(effect, AL_REVERB_DIFFUSION, reverb->flDiffusion);
		alEffectf(effect, AL_REVERB_GAIN, reverb->flGain);
		alEffectf(effect, AL_REVERB_GAINHF, reverb->flGainHF);
		alEffectf(effect, AL_REVERB_DECAY_TIME, reverb->flDecayTime);
		alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
		alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
		alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
		alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
		alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
		alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
		alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
		alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
	}

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
		if(alIsEffect(effect))
			alDeleteEffects(1, &effect);
		return 0;
	}

	return effect;
}
