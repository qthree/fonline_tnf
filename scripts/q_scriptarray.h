struct qScriptArray
{
	void*  VTable;
	
	//void   Resize(asUINT numElements);
	
	int    RefCount;
	bool   GCFlag;
	void*  ObjType; // asIObjectType

	struct SArrayBuffer
	{
		uint   NumElements;
		uint8  Data[1];
	} *Buffer;

	bool   IsArrayOfHandles;
	int    ElementSize;
	int    CmpFuncId;
	int    EqFuncId;
	int    SubTypeId;

	uint   GetSize()      {return Buffer->NumElements;}
	void*  At(uint index) {return Buffer->Data + index * ElementSize;}
};