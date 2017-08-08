/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileReadBytestream.h>

#include <cassert>
#include <memory>

#include <Swiften/Base/Log.h>
#include <boost/filesystem/fstream.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/ByteArray.h>

namespace Swift {

FileReadBytestream::FileReadBytestream(const boost::filesystem::path& file) : file(file), stream(nullptr) {
}

FileReadBytestream::~FileReadBytestream() {
    if (stream) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}

std::shared_ptr<ByteArray> FileReadBytestream::read(size_t size)  {
    if (!stream) {
        stream = new boost::filesystem::ifstream(file, std::ios_base::in|std::ios_base::binary);
    }
    std::shared_ptr<ByteArray> result = std::make_shared<ByteArray>();
    result->resize(size);
    assert(stream->good());
    stream->read(reinterpret_cast<char*>(vecptr(*result)), boost::numeric_cast<std::streamsize>(size));
    result->resize(boost::numeric_cast<size_t>(stream->gcount()));
    onRead(*result);
    return result;
}

///hero
void FileReadBytestream::close()
{
	SWIFT_LOG(debug) << "Close Stream" << stream;
	if (stream) {
		stream->close();
		stream = nullptr;
	}
}

///hero
void FileReadBytestream::seek(const boost::uintmax_t offset, ReadBytestream::SeekType seekType)
{
	if (!stream) {
		stream = new boost::filesystem::ifstream(file, std::ios_base::in | std::ios_base::binary);
	}
	assert(stream->good());

	std::ifstream::seekdir offType;
	switch (seekType)
	{
	case ReadBytestream::beg:
		offType = std::ifstream::beg;
		break;
	case ReadBytestream::cur:
		offType = std::ifstream::cur;
		break;
	case ReadBytestream::end:
		offType = std::ifstream::end;
		break;
	default:
		offType = std::ifstream::beg;
		break;
	}
	stream->seekg(offset, offType);
}

bool FileReadBytestream::isFinished() const {
    return stream && !stream->good();
}

}
