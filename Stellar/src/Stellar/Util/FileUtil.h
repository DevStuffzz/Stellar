#pragma once

#include "Core.h"

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace Stellar::File {

    using Path = std::filesystem::path;

    // File information
    ST_API bool Exists(const Path& path);
    ST_API bool IsFile(const Path& path);
    ST_API bool IsDirectory(const Path& path);

    ST_API uintmax_t GetSize(const Path& path);

    ST_API std::string GetExtension(const Path& path);
    ST_API std::string GetName(const Path& path);
    ST_API std::string GetStem(const Path& path);

    ST_API Path GetParent(const Path& path);
    ST_API Path GetAbsolutePath(const Path& path);

    // Reading
    ST_API std::string ReadText(const Path& path);
    ST_API std::vector<uint8_t> ReadBinary(const Path& path);

    // Writing
    ST_API bool WriteText(
        const Path& path,
        const std::string& content);

    ST_API bool AppendText(
        const Path& path,
        const std::string& content);

    ST_API bool WriteBinary(
        const Path& path,
        const void* data,
        size_t size);

    ST_API bool WriteBinary(
        const Path& path,
        const std::vector<uint8_t>& data);

    // File operations
    ST_API bool Delete(const Path& path);

    ST_API bool Copy(
        const Path& source,
        const Path& destination,
        bool overwrite = false);

    ST_API bool Move(
        const Path& source,
        const Path& destination);

    // Directory operations
    ST_API bool CreateDirectory(const Path& path);
    ST_API bool DeleteDirectory(const Path& path);

    ST_API std::vector<Path> GetFiles(
        const Path& directory,
        bool recursive = false);

    ST_API std::vector<Path> GetDirectories(
        const Path& directory,
        bool recursive = false);

}