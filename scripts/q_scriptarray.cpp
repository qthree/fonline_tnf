#include <new>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h> // sprintf

#include "q_scriptarray.h"

void qScriptArray::Resize(asUINT numElements)
{
	if( numElements & 0x80000000 )
	{
		CheckMaxSize(numElements);
		return;
	}

	Resize((int)numElements - (int)buffer->numElements, (asUINT)-1);
}

// Internal
void qScriptArray::Resize(int delta, asUINT at)
{
	if( delta < 0 )
	{
		if( -delta > (int)buffer->numElements )
			delta = -(int)buffer->numElements;
		if( at > buffer->numElements + delta )
			at = buffer->numElements + delta;
	}
	else if( delta > 0 )
	{
		// Make sure the array size isn't too large for us to handle
		if( delta > 0 && !CheckMaxSize(buffer->numElements + delta) )
			return;

		if( at > buffer->numElements )
			at = buffer->numElements;
	}

	if( delta == 0 ) return;

	// Allocate memory for the buffer
	SArrayBuffer *newBuffer;
	newBuffer = (SArrayBuffer*)new asBYTE[sizeof(SArrayBuffer)-1 + elementSize*(buffer->numElements + delta)];
	newBuffer->numElements = buffer->numElements + delta;

	memcpy(newBuffer->data, buffer->data, at*elementSize);
	if( delta > 0 && at < buffer->numElements )
		memcpy(newBuffer->data + (at+delta)*elementSize, buffer->data + at*elementSize, (buffer->numElements-at)*elementSize);
	else if( delta < 0 && at < buffer->numElements )
		memcpy(newBuffer->data + at*elementSize, buffer->data + (at-delta)*elementSize, (buffer->numElements-at+delta)*elementSize);

	int typeId = objType->GetSubTypeId();
	if( typeId & asTYPEID_MASK_OBJECT )
	{
		if( delta > 0 )
			Construct(newBuffer, at, at+delta);
		else if( delta < 0 )
			Destruct(buffer, at, at-delta);
	}

	// Release the old buffer
	delete[] (asBYTE*)buffer;

	buffer = newBuffer;
}

bool qScriptArray::CheckMaxSize(asUINT numElements)
{
	// This code makes sure the size of the buffer that is allocated 
	// for the array doesn't overflow and becomes smaller than requested

	asUINT maxSize = 0xFFFFFFFFul - sizeof(SArrayBuffer) + 1;
	if( objType->GetSubTypeId() & asTYPEID_MASK_OBJECT )
		maxSize /= sizeof(void*);
	else if( elementSize > 0 )
		maxSize /= elementSize;

	if( numElements > maxSize )
	{
		asIScriptContext *ctx = asGetActiveContext();
		if( ctx )
		{
			// Set a script exception
			ctx->SetException("Too large array size");
		}

		return false;
	}

	// OK
	return true;
}