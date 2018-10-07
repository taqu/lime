#include <catch_wrap.hpp>
#include "VFSPack.h"
#include "FileSystem.h"
#include "VirtualFileSystem.h"

namespace lcore
{
    TEST_CASE("TestFileSystem::testVirtualFileSystemOS")
    {
        VirtualFileSystemOS vfs("data");

        EXPECT_EQ(vfs.getType(), fs::VFSType_OS);

        fs::FileStatus status;
        s32 index;
        DirectoryProxy* root = vfs.openRoot();
        CHECK(NULL != root);
        EXPECT_EQ(root->getNumChildren(), 2);
        EXPECT_GT(0, root->findChild("."));
        EXPECT_GT(0, root->findChild(".."));
        EXPECT_LE(0, root->findChild("directory00"));
        EXPECT_LE(0, root->findChild("file00.txt"));
        EXPECT_GT(0, root->findChild("file01.txt"));

        index = root->findChild("directory00");
        status = root->getStatus(index);
        EXPECT_EQ(status.type_, fs::Type_Directory);
        EXPECT_STR_EQ(status.name_, "directory00");

        index = root->findChild("file00.txt");
        status = root->getStatus(index);
        EXPECT_EQ(status.type_, fs::Type_File);
        EXPECT_STR_EQ(status.name_, "file00.txt");

        DirectoryProxy* directory00 = root->openDirectory(root->findChild("directory00"));
        CHECK(NULL != directory00);
        vfs.closeDirectory(root);

        FileProxy* file01 = directory00->openFile(directory00->findChild("file01.txt"));
        CHECK(NULL != file01);
        FileProxy* file02 = vfs.openFile("directory00/directory01/file02.txt");
        CHECK(NULL != file02);
        vfs.closeDirectory(directory00);

        vfs.closeFile(file01);
        vfs.closeFile(file02);
    }

#if 0
    TEST_CASE("TestFileSystem::testWritePack")
    {
        bool result;
        result = lcore::writeVFSPack("data.lpak", "data");
        CHECK(result);
    }
#endif

    TEST_CASE("TestFileSystem::testVirtualFileSystemPack")
    {
        bool result;
        VFSPack vfsPack;
        result = lcore::readVFSPack(vfsPack, "data.lpak", true);
        CHECK(result);

        VirtualFileSystemPack vfs(vfsPack);

        EXPECT_EQ(vfs.getType(), fs::VFSType_Pack);

        fs::FileStatus status;
        s32 index;
        DirectoryProxy* root = vfs.openRoot();
        CHECK(NULL != root);
        EXPECT_EQ(root->getNumChildren(), 2);
        EXPECT_GT(0, root->findChild("."));
        EXPECT_GT(0, root->findChild(".."));
        EXPECT_LE(0, root->findChild("directory00"));
        EXPECT_LE(0, root->findChild("file00.txt"));
        EXPECT_GT(0, root->findChild("file01.txt"));

        index = root->findChild("directory00");
        status = root->getStatus(index);
        EXPECT_EQ(status.type_, fs::Type_Directory);
        EXPECT_STR_EQ(status.name_, "directory00");

        index = root->findChild("file00.txt");
        status = root->getStatus(index);
        EXPECT_EQ(status.type_, fs::Type_File);
        EXPECT_STR_EQ(status.name_, "file00.txt");

        DirectoryProxy* directory00 = root->openDirectory(root->findChild("directory00"));
        CHECK(NULL != directory00);
        vfs.closeDirectory(root);

        FileProxy* file01 = directory00->openFile(directory00->findChild("file01.txt"));
        CHECK(NULL != file01);
        FileProxy* file02 = vfs.openFile("directory00/directory01/file02.txt");
        CHECK(NULL != file02);
        vfs.closeDirectory(directory00);

        vfs.closeFile(file01);
        vfs.closeFile(file02);
    }

    TEST_CASE("TestFileSystem::testFileSystem")
    {
        u8 buffer[1024];
        bool result;
        FileSystem fileSystem;
        result = fileSystem.mountOS(0, "data");
        CHECK(result);
        result = fileSystem.mountPack(1, "data.lpak", true);
        CHECK(result);

        FileProxy* file00 = fileSystem.openFile("file00.txt");
        CHECK(NULL != file00);
        EXPECT_EQ(file00->getUncompressedSize(), 8);
        CHECK(file00->read(0, file00->getUncompressedSize(), buffer));
        buffer[8] = lcore::CharNull;
        EXPECT_STR_EQ(reinterpret_cast<Char*>(buffer), "file00\r\n");
        fileSystem.closeFile(file00);
    }

    TEST_CASE("TestFileSystem::testFileStream")
    {
        u8 buffer[1024];

        bool result;
        FileSystem fileSystem;
        result = fileSystem.mountOS(0, "data");
        CHECK(result);
        result = fileSystem.mountPack(1, "data.lpak", true);
        CHECK(result);

        FileProxy* file00 = fileSystem.openFile("file00.txt");
        CHECK(NULL != file00);

        lcore::FileProxyIStream stream(file00);
        CHECK_FALSE(stream.eof());
        CHECK(stream.good());
        CHECK(stream.seekg(0, lcore::ios::beg));
        EXPECT_EQ(stream.tellg(), 0);
        EXPECT_EQ(stream.getSize(), file00->getUncompressedSize());
        s32 count = 0;
        while(!stream.eof()){
            stream.read(1, buffer+count);
            ++count;
        }
        EXPECT_EQ(count, 8);
        buffer[count] = lcore::CharNull;
        EXPECT_STR_EQ(reinterpret_cast<Char*>(buffer), "file00\r\n");
        fileSystem.closeFile(file00);
    }
}
