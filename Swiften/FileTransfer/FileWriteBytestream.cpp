/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileWriteBytestream.h>

#include <cassert>

#include <boost/filesystem/fstream.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace Swift {

FileWriteBytestream::FileWriteBytestream(const boost::filesystem::path& file) : file(file), stream(nullptr) {
}

FileWriteBytestream::~FileWriteBytestream() {
    if (stream) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

bool FileWriteBytestream::write(const std::vector<unsigned char>& data) {
    if (data.empty()) {
        return true;
    }
    if (!stream) {
        stream = new boost::filesystem::ofstream(file, std::ios_base::out|std::ios_base::binary);
    }
    if (stream->good()) {
        stream->write(reinterpret_cast<const char*>(&data[0]), boost::numeric_cast<std::streamsize>(data.size()));
        if (stream->good()) {
            onWrite(data);
            return true;
        }
    }
    return false;
}

void FileWriteBytestream::close() {
    if (stream) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

///hero
void FileWriteBytestream::seek(const boost::uintmax_t offset, WriteBytestream::SeekType seekType) 
{
	if (!stream) {
 
		stream = new boost::filesystem::ofstream(file, std::ios_base::app | std::ios_base::binary);
	}
	//assert(stream->good());

	std::ifstream::seekdir offType;
	switch (seekType)
	{
	case WriteBytestream::beg:
		offType = std::ifstream::beg;
		break;
	case WriteBytestream::cur:
		offType = std::ifstream::cur;
		break;
	case WriteBytestream::end:
		offType = std::ifstream::end;
		break;
	default:
		offType = std::ifstream::beg;
		break;
	}

	stream->seekp(offset, offType);
}

}
