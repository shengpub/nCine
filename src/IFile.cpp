#include <cstdlib> // for getenv()
#include <cstring> // for strlen()
#include "common_macros.h"
#include "IFile.h"
#include "StandardFile.h"

#ifdef __ANDROID__
	#include <sys/stat.h> // for mkdir()
	#include <errno.h> // for EEXIST
	#include "AndroidApplication.h"
	#include "AssetFile.h"
#endif

#ifndef _WIN32
	#include <unistd.h> // for getpid()
	#include <pwd.h> // for getpwuid()
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::String IFile::dataPath_(MaxFilenameLength);
nctl::String IFile::savePath_(MaxFilenameLength);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IFile::IFile(const char *filename)
    : type_(FileType::BASE), filename_(filename), extension_(MaxExtensionLength),
      fileDescriptor_(-1), filePointer_(nullptr), shouldCloseOnDestruction_(true),
      shouldExitOnFailToOpen_(true), fileSize_(0)
{
	ASSERT(filename);

	int dotChar = filename_.findLastChar('.');
	// A dot followed by at least three extension characters
	if (dotChar && (filename_.length() - dotChar) >= 4)
		extension_.assign(filename_, dotChar + 1, filename_.length() - dotChar - 1);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool IFile::isOpened() const
{
	if (fileDescriptor_ >= 0 || filePointer_ != nullptr)
		return true;
	else
		return false;
}

bool IFile::hasExtension(const char *extension) const
{
	ASSERT(extension);
	return extension_ == extension;
}

nctl::UniquePtr<IFile> IFile::createFileHandle(const char *filename)
{
	ASSERT(filename);
#ifdef __ANDROID__
	if (strncmp(filename, static_cast<const char *>("asset::"), 7) == 0)
		return nctl::makeUnique<AssetFile>(filename + 7);
	else
#endif
		return nctl::makeUnique<StandardFile>(filename);
}

bool IFile::access(const char *filename, unsigned char mode)
{
	ASSERT(filename);
#ifdef __ANDROID__
	if (strncmp(filename, static_cast<const char *>("asset::"), 7) == 0)
		return AssetFile::access(filename + 7, mode);
	else
#endif
		return StandardFile::access(filename, mode);
}

const nctl::String &IFile::savePath()
{
	if (savePath_.isEmpty())
		initSavePath();

	return savePath_;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void IFile::initSavePath()
{
#ifdef __ANDROID__
	AndroidApplication &application = static_cast<AndroidApplication &>(theApplication());

	// Get the internal data path from the Android application
	savePath_ = application.internalDataPath();

	// Trying to create the data directory
	if (mkdir(savePath_.data(), 0770) && errno != EEXIST)
	{
		LOGE_X("Cannot create directory: %s", savePath_.data());
		savePath_.clear();
	}
#elif _WIN32
	const char *userProfileEnv = getenv("USERPROFILE");
	if (userProfileEnv == nullptr || strlen(userProfileEnv) == 0)
	{
		const char *homeDriveEnv = getenv("HOMEDRIVE");
		const char *homePathEnv = getenv("HOMEPATH");

		if ((homeDriveEnv == nullptr || strlen(homeDriveEnv) == 0) &&
		    (homePathEnv == nullptr || strlen(homePathEnv) == 0))
		{
			const char *homeEnv = getenv("HOME");
			if (homeEnv && strlen(homeEnv))
				savePath_ = homeEnv;
		}
		else
		{
			savePath_ = homeDriveEnv;
			savePath_ += homePathEnv;
		}
	}
	else
		savePath_ = userProfileEnv;

	if (savePath_.isEmpty() == false)
		savePath_ += "\\";
#else
	const char *homeEnv = getenv("HOME");

	if (homeEnv == nullptr || strlen(homeEnv) == 0)
		savePath_ = getpwuid(getuid())->pw_dir;
	else
		savePath_ = homeEnv;

	savePath_ += "/.config/";
#endif
}

}
