///////////////////////////////////////////////////////////////////////////
//
//	SDL_CheckSB() - Checks to see if a SoundBlaster resides at a
//		particular I/O location
//
///////////////////////////////////////////////////////////////////////////
static _boolean
SDL_CheckSB(int16_t port)
{
	int16_t	i;

	sbLocation = port << 4;		// Initialize stuff for later use

	sbOut(sbReset,_true);		// Reset the SoundBlaster DSP
asm	mov	dx,0x388				// Wait >4usec
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

	sbOut(sbReset,_false);		// Turn off sb DSP reset
asm	mov	dx,0x388				// Wait >100usec
asm	mov	cx,100
usecloop:
asm	in	al,dx
asm	loop usecloop

	for (i = 0;i < 100;i++)
	{
		if (sbIn(sbDataAvail) & 0x80)		// If data is available...
		{
			if (sbIn(sbReadData) == 0xaa)	// If it matches correct value
				return(_true);
			else
			{
				sbLocation = -1;			// Otherwise not a SoundBlaster
				return(_false);
			}
		}
	}
	sbLocation = -1;						// Retry count exceeded - fail
	return(_false);
}

///////////////////////////////////////////////////////////////////////////
//
//	Checks to see if a SoundBlaster is in the system. If the port passed is
//		-1, then it scans through all possible I/O locations. If the port
//		passed is 0, then it uses the default (2). If the port is >0, then
//		it just passes it directly to SDL_CheckSB()
//
///////////////////////////////////////////////////////////////////////////
static _boolean
SDL_DetectSoundBlaster(int16_t port)
{
	int16_t	i;

	if (port == 0)					// If user specifies default, use 2
		port = 2;
	if (port == -1)
	{
		if (SDL_CheckSB(2))			// Check default before scanning
			return(_true);

		if (SDL_CheckSB(4))			// Check other SB Pro location before scan
			return(_true);

		for (i = 1;i <= 6;i++)		// Scan through possible SB locations
		{
			if ((i == 2) || (i == 4))
				continue;

			if (SDL_CheckSB(i))		// If found at this address,
				return(_true);		//	return success
		}
		return(_false);				// All addresses failed, return failure
	}
	else
		return(SDL_CheckSB(port));	// User specified address or default
}

