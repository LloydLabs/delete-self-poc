# 🗑️ delete-self-poc
The `delete-self-poc` is a demonstration of a method to delete a locked executable or currently running file from disk. This concept was initially discovered by Jonas Lykkegaard, and I have created the proof of concept (POC) for it. Additionally, it can be used to delete locked files on disk, provided that the current calling process has the necessary permissions to access and delete them.

How does this work, though - in this POC?
1. Open a HANDLE to the current running process with DELETE access. Note that only DELETE access is required.
2. Use the SetFileInformationByHandle function to rename the primary file stream, :$DATA, to :wtfbbq.
3. Close the HANDLE.
4. Open a HANDLE to the current process and set the DeleteFile flag of the FileDispositionInfo class to TRUE.
5. Close the HANDLE to trigger the file disposition.
6. Voila! The file is now gone.

# Releases
I have included a statically linked release within this repository, if you can't be bothered compiling the original source code.
