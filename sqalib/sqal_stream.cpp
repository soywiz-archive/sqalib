#include "sqal_internal.h"

class Stream { public:
	enum Origin {
		Set = SEEK_SET,
		Cur = SEEK_CUR,
		End = SEEK_END,
	};
	
	int read(u8 *outputBuffer, int length) {
		if (outputBuffer == NULL) {
			i64 tell_back = tell();
			{
				seek(length, Stream::Cur);
			}
			return tell() - tell_back;
		} else {
			return _read(outputBuffer, length);
		}
	}

	int write(u8 *inputBuffer, int length = -1) {
		if (inputBuffer == NULL) return 0;
		if (length == -1) length = strlen((char *)inputBuffer);
		return _write(inputBuffer, length);
	}
	
	i64 seek(i64 setPosition, Origin origin = Set) {
		return _seek(setPosition, origin);
	}

	i64 tell() { return seek(0, Stream::Cur); }
	void truncate() { return truncate(tell()); }
	void truncate(i64 setPosition) {
		return _truncate(setPosition);
	}
	
	i64 size() {
		i64 cursor = tell();
		i64 _size = seek(0, End);
		seek(cursor);
		return _size;
	}
	
	void flush() { _flush(); }

protected:
	virtual int _read(u8 *outputBuffer, int outputLength) = NULL;
	virtual int _write(u8 *inputBuffer, int inputLength) = NULL;
	virtual i64 _seek(i64 setPosition, Origin origin) = NULL;
	virtual void _truncate(i64 setPosition) { }
	virtual void _flush() { }
};

class FileStream : public Stream { public:
	FILE *file;

	FileStream(char *name, char *mode) {
		file = fopen(name, mode);
		assert(file != NULL);
	}
	
	~FileStream() {
		fclose(file);
	}

	virtual int _read(u8 *outputBuffer, int outputLength) {
		return fread(outputBuffer, 1, outputLength, file);
	}

	virtual int _write(u8 *inputBuffer, int inputLength) {
		return fwrite(inputBuffer, 1, inputLength, file);
	}
	
	virtual i64 _seek(i64 setPosition, Origin origin) {
		fseek(file, setPosition, origin);
		return ftell(file);
	}

	virtual void _truncate(i64 setPosition) {
		//ftruncate(file, setPosition);
	}

	virtual void _flush() {
		fflush(file);
	}
};

class MemoryStream : public Stream { public:
	u8 *memory;
	int position;
	int length, capacity;
	bool growable, freeMemory;
	
	MemoryStream(int length = 0, u8 *buffer = NULL, bool growable = true, bool freeMemory = true) {
		if (buffer == NULL) buffer = (u8 *)malloc(length);
		{
			this->memory = buffer;
			this->position = 0;
			this->capacity = this->length = length;
		}
		this->growable = growable;
		this->freeMemory = freeMemory;

		assert(this->memory != NULL);
	}
	
	~MemoryStream() {
		if (freeMemory && (memory != NULL)) free(memory);
	}
	
	void asserts() {
		assert((memory != NULL) && (length > 0));
		assert(position >= 0);
		assert(position <= length);
		assert(length <= capacity);
	}
	
	void clampLength(int &curLength, int lengthUse) {
		if (position + curLength > lengthUse) length = lengthUse - position;
	}
	
	void clampPosition() {
		if (position < 0) position = 0;
		if (this->growable) growLength(position);
		if (position > length) position = length;
	}
	
	void growLength(int lengthExpected) {
		if (!this->growable) return;
		if (lengthExpected <= this->capacity) return;

		this->memory = (u8 *)realloc(this->memory, lengthExpected);
		this->capacity = this->length = lengthExpected;

		assert(this->memory != NULL);
	}

	virtual int _read(u8 *outputBuffer, int outputLength) {
		clampLength(outputLength, length);
		{
			memcpy(outputBuffer, memory + position, outputLength);
			_seek(outputLength, Cur);
		}
		return outputLength;
	}

	virtual int _write(u8 *inputBuffer, int inputLength) {
		growLength(position + inputLength);
		clampLength(inputLength, capacity);
		{
			memcpy(memory + position, inputBuffer, inputLength);
			_seek(inputLength, Cur);
		}
		clampPosition();
		return inputLength;
	}
	
	virtual i64 _seek(i64 setPosition, Origin origin) {
		switch (origin) {
			case Set: position = setPosition; break;
			case Cur: position += setPosition; break;
			case End: position = length + setPosition; break;
		}
		clampPosition();
		return position;
	}

	virtual void _truncate(i64 setPosition) {
		growLength(setPosition);
		this->length = setPosition;
	}
};

class SliceStream : public Stream { public:
	Stream *parentStream;
	i64 base, length;
	i64 position;
	bool deleteParent;

	SliceStream(Stream *parent, i64 base = 0, i64 length = -1, bool deleteParent = false) {
		if (base < 0) base = 0;
		if (base > parent->size()) base = parent->size();
		if (length == -1) length = parent->size();
		if (base + length > parent->size()) length = parent->size() - base;
		{
			this->parentStream = parent;
			this->base = base;
			this->length = length;
			this->position = 0;
			this->deleteParent = deleteParent;
		}
		assert(this->parentStream != NULL);
		assert(this->base >= 0);
		assert(this->length >= 0);
	}
	
	~SliceStream() {
		if (deleteParent) delete parentStream;
	}
	
	void clampPosition() {
		if (position < 0) position = 0;
		else if (position >= length) position = length;
	}

	virtual int _read(u8 *outputBuffer, int outputLength) {
		int length;
		i64 back_pos = parentStream->tell();
		parentStream->seek(base + position);
		{
			length = parentStream->read(outputBuffer, outputLength);
			seek(length, Cur);
		}
		parentStream->seek(back_pos);
		clampPosition();
		return length;
	}

	virtual int _write(u8 *inputBuffer, int inputLength) {
		int length;
		i64 back_pos = parentStream->tell();
		parentStream->seek(base + position);
		{
			length = parentStream->write(inputBuffer, inputLength);
			seek(length, Cur);
		}
		parentStream->seek(back_pos);
		clampPosition();
		return length;
	}

	virtual i64 _seek(i64 setPosition, Origin origin) {
		switch (origin) {
			case Set: position = setPosition; break;
			case Cur: position += setPosition; break;
			case End: position = length + setPosition; break;
		}
		clampPosition();
		return position;
	}
};

struct FileInfo {
	char *name;
	u64 size;
	u64 ctime, mtime, atime;
	u64 uid, gid, flags;
};

class DirectoryList {
	FileInfo *list;
	int length, capacity;

	DirectoryList() {
	}

	~DirectoryList() {
	}
};

class Directory { public:
	bool unlink(char *name) {
		return false;
	}
	
	bool exists(char *name) {
		return false;
	}
	
	Stream *open(char *name) {
		return NULL;
	}
	
	bool mkdir(char *name) {
		return false;
	}

	bool rmdir(char *name) {
		return false;
	}
	
	DirectoryList *list(char *name = NULL) {
		return NULL;
	}
};

unittest(Stream) {
	u8 temp[16];

	Stream *mem = new MemoryStream();
	mem->write((u8 *)"Test");
	unit_assert(mem->size() == 4);
	
	Stream *slice = new SliceStream(mem, 1, 2);
	unit_assert(slice->size() == 2);
	unit_assert(slice->tell() == 0);
	unit_assert(slice->read(temp, 2) == 2)
	unit_assert(slice->tell() == 2);
	unit_assert(memcmp("es", temp, 2) == 0);

	delete slice;
	delete mem;
}
