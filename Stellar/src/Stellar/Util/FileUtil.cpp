#include "FileUtil.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace Stellar::File {

    namespace fs = std::filesystem;

    // File information

    bool Exists(const Path& path)
    {
        return fs::exists(path);
    }

    bool IsFile(const Path& path)
    {
        return fs::is_regular_file(path);
    }

    bool IsDirectory(const Path& path)
    {
        return fs::is_directory(path);
    }

    uintmax_t GetSize(const Path& path)
    {
        return fs::file_size(path);
    }

    std::string GetExtension(const Path& path)
    {
        return path.extension().string();
    }

    std::string GetName(const Path& path)
    {
        return path.filename().string();
    }

    std::string GetStem(const Path& path)
    {
        return path.stem().string();
    }

    Path GetParent(const Path& path)
    {
        return path.parent_path();
    }

    Path GetAbsolutePath(const Path& path)
    {
        return fs::absolute(path);
    }


    // Reading

    std::string ReadText(const Path& path)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
            throw std::runtime_error(
                "Failed to open file: " + path.string());

        std::ostringstream stream;
        stream << file.rdbuf();

        if (file.bad() || stream.bad())
            throw std::runtime_error(
                "Failed to read file: " + path.string());

        return stream.str();
    }

    std::vector<uint8_t> ReadBinary(const Path& path)
    {
        std::ifstream file(
            path, std::ios::binary | std::ios::ate);

        if (!file)
            throw std::runtime_error(
                "Failed to open file: " + path.string());

        const auto size = file.tellg();

        if (size < 0)
            throw std::runtime_error("Invalid file size");

        std::vector<uint8_t> buffer(
            static_cast<size_t>(size));

        file.seekg(0, std::ios::beg);

        if (!buffer.empty())
        {
            if (!file.read(
                reinterpret_cast<char*>(buffer.data()),
                static_cast<std::streamsize>(buffer.size())))
            {
                throw std::runtime_error(
                    "Failed to read file: " + path.string());
            }
        }

        return buffer;
    }


    // Writing

    bool WriteText(
        const Path& path,
        const std::string& content)
    {
        std::ofstream file(
            path, std::ios::binary | std::ios::trunc);

        if (!file)
            return false;

        file << content;
        file.close();

        return !file.fail();
    }

    bool AppendText(
        const Path& path,
        const std::string& content)
    {
        std::ofstream file(
            path, std::ios::binary | std::ios::app);

        if (!file)
            return false;

        file << content;
        file.close();

        return !file.fail();
    }

    bool WriteBinary(
        const Path& path,
        const void* data,
        size_t size)
    {
        if (!data && size > 0)
            return false;

        std::ofstream file(
            path, std::ios::binary | std::ios::trunc);

        if (!file)
            return false;

        if (size > 0)
        {
            file.write(
                static_cast<const char*>(data),
                static_cast<std::streamsize>(size));
        }

        file.close();

        return !file.fail();
    }

    bool WriteBinary(
        const Path& path,
        const std::vector<uint8_t>& data)
    {
        return WriteBinary(
            path, data.data(), data.size());
    }


    // File operations

    bool Delete(const Path& path)
    {
        std::error_code ec;
        return fs::remove(path, ec);
    }

    bool Copy(
        const Path& source,
        const Path& destination,
        bool overwrite)
    {
        std::error_code ec;

        return fs::copy_file(
            source,
            destination,
            overwrite
            ? fs::copy_options::overwrite_existing
            : fs::copy_options::none,
            ec);
    }

    bool Move(
        const Path& source,
        const Path& destination)
    {
        std::error_code ec;

        fs::rename(source, destination, ec);

        return !ec;
    }


    // Directory operations

    bool CreateDirectory(const Path& path)
    {
        std::error_code ec;

        fs::create_directories(path, ec);

        return !ec;
    }

    bool DeleteDirectory(const Path& path)
    {
        std::error_code ec;

        if (!fs::is_directory(path, ec) || ec)
            return false;

        fs::remove_all(path, ec);

        return !ec;
    }

    std::vector<Path> GetFiles(
        const Path& directory,
        bool recursive)
    {
        std::vector<Path> files;

        if (recursive)
        {
            for (const auto& entry :
                fs::recursive_directory_iterator(directory))
            {
                if (entry.is_regular_file())
                    files.push_back(entry.path());
            }
        }
        else
        {
            for (const auto& entry :
                fs::directory_iterator(directory))
            {
                if (entry.is_regular_file())
                    files.push_back(entry.path());
            }
        }

        return files;
    }

    std::vector<Path> GetDirectories(
        const Path& directory,
        bool recursive)
    {
        std::vector<Path> directories;

        if (recursive)
        {
            for (const auto& entry :
                fs::recursive_directory_iterator(directory))
            {
                if (entry.is_directory())
                    directories.push_back(entry.path());
            }
        }
        else
        {
            for (const auto& entry :
                fs::directory_iterator(directory))
            {
                if (entry.is_directory())
                    directories.push_back(entry.path());
            }
        }

        return directories;
    }

}