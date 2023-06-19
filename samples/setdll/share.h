#ifndef COPYFILE_H
#define COPYFILE_H

#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

string GetFilePathWithoutFileName(string filePath) {
    replace(filePath.begin(), filePath.end(), '/', '\\');
    int separatorPos = (int)min(filePath.rfind('/'), filePath.rfind('\\'));
    if (separatorPos == string::npos) {
        return ""; 
    }
    return filePath.substr(0, separatorPos) + "\\";
}

bool CopyFile(const char* sourcePath, const char* destPath)
{
    bool success = true;
    ifstream source(sourcePath, ios::binary);
    ofstream dest(destPath, ios::binary);
    if (source.fail()) {
        cout << "Failed to open source file: " << sourcePath << endl;
        success = false;
    }
    if (dest.fail()) {
        cout << "Failed to open destination file: " << destPath << endl;
        success = false;
    }
    if (success) {
        char buffer[1024];
        while (!source.eof() && !source.fail()) {
            source.read(buffer, sizeof(buffer));
            int count = (int)source.gcount();
            dest.write(buffer, count);
            if (dest.fail()) {
                cout << "Failed to write to destination file: " << destPath << endl;
                success = false;
                break;
            }
        }
    }
    source.close();
    dest.close();
    return success;
}

#endif  // COPYFILE_H
