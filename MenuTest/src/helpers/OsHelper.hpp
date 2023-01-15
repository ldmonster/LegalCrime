#ifdef _WIN32
#include <string>
#include <vector>
#include <windows.h>

// Returns an vector of filenames in the directory specified by 'path', 
std::vector<std::string> directory_files(std::string path, bool useFolderPrefix)
{
    HANDLE hFind;
    WIN32_FIND_DATAA data;

    std::string strPath = path + "*";
    std::vector<std::string> pathVec;

    // Count Files
    hFind = FindFirstFileA(strPath.c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // ignored all files starting by "." and directories like ".."
            if (data.cFileName[0] == '.')
            {
                continue;
            }

            std::string fileName = data.cFileName;;

            if (useFolderPrefix)
            {
                fileName = path + fileName;
            }

            pathVec.push_back(fileName);
        } while (FindNextFileA(hFind, &data));

        FindClose(hFind);
    }
    else 
    {
        printf("invalid");
    }

    return pathVec;
}

void free_directory_files(char** filenames, size_t size)
{
    if (filenames)
    {
        for (size_t i = 0; i < size; ++i)
            free(filenames[i]);

        free(filenames);
    }
}

#endif // _WIN32
