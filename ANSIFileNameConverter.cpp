#include <iostream>
#include <filesystem>
#include <regex>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <typeinfo>

#include <windows.h>
#include <shlobj.h>

#include "FileCounters.h"

namespace fs = std::filesystem;

std::string openFolderBrowser()
{
	wchar_t folderPath[MAX_PATH] = { 0 };

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = L"Select a folder to process:";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != nullptr)
	{
		SHGetPathFromIDList(pidl, folderPath);
		CoTaskMemFree(pidl);

		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, folderPath, -1, nullptr, 0, nullptr, nullptr);
		if (bufferSize > 0)
		{
			std::string folderPathStr(bufferSize - 1, '\0');
			WideCharToMultiByte(CP_UTF8, 0, folderPath, -1, &folderPathStr[0], bufferSize, nullptr, nullptr);
			return folderPathStr;
		}
	}

	return "";
}

bool renameFile(const fs::path& filePath, const std::wstring& newName)
{
	try
	{
		fs::rename(filePath, filePath.parent_path() / newName);
		return true;
	}
	catch (const fs::filesystem_error&)
	{
		return false;
	}
}

bool checkFolderExists(const std::string& folder)
{
	if (!fs::exists(folder) || !fs::is_directory(folder))
	{
		std::cerr << "Error: The specified folder does not exist." << std::endl;
		return false;
	}
	return true;
}

void renameFilesInFolder(const std::string& sourceFolder)
{
	if (!checkFolderExists(sourceFolder))
	{
		return;
	}

	for (const auto& entry : fs::directory_iterator(sourceFolder))
	{
		if (entry.is_regular_file())
		{
			std::wstring oldName = entry.path().filename().wstring();
			std::wregex nonAsciiRegex(LR"([^ -~])");
			std::wstring newName = std::regex_replace(oldName, nonAsciiRegex, L"_");

			if (oldName != newName)
			{
				if (renameFile(entry.path(), newName))
				{
					FileCounters::incrementChangedFileCount();
				}
				else
				{
					FileCounters::incrementFailedFileCount();
				}
			}
			else
			{
				FileCounters::incrementUnchangedFileCount();
			}
		}
	}

	std::cout << FileCounters::getCounters() << std::endl;
	//std::wcout << L"File renaming completed in " << std::wstring(sourceFolder.begin(), sourceFolder.end()) << std::endl;
}

int main(int argc, char* argv[])
{
	std::string sourceFolder;

	if (argc != 2)
	{
		std::cout << "No folder path provided. Opening folder browser..." << std::endl;
		sourceFolder = openFolderBrowser();

		if (sourceFolder.empty())
		{
			std::cerr << "No folder selected. Exiting program." << std::endl;
			return 0;
		}
	}
	else
	{
		sourceFolder = argv[1];
	}

	renameFilesInFolder(sourceFolder);

	return 1;
}
