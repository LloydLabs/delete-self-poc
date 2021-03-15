# 🗑️ delete-self-poc
A way to delete a locked, or current running executable, on disk. This was originally found by Jonas Lykkegaard - I just wrote the POC for it. This can also be used to delete locked files on disk, that the current calling process has permissions to get `DELETE` access to. 

How does this work, though - in this POC?
1. Open a `HANDLE` to the current running process, with `DELETE` access. Note, `DELETE` is only needed.
2. Rename the primary file stream, `:$DATA`, using `SetFileInformationByHandle` to `:wtfbbq`.
3. Close the `HANDLE`
4. Open a `HANDLE` to the current process, set `DeleteFile` for the `FileDispositionInfo` class to `TRUE`.
5. Close the `HANDLE` to trigger the file disposition
6. Viola - the file is gone.

# Releases
I have included a statically linked release within this repository, if you can't be bothered compiling the original source code.
