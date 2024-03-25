# Simple-Network-File-system
What is NFS?
Network File System (NFS) is a distributed file system protocol that allows a user on a client computer to access files over a network as if they were stored locally. NFS was originally developed by Sun Microsystems in the 1980s and has since become a widely used protocol for sharing files and directories among UNIX-like systems.

How does NFS work?
NFS operates on the client-server model. The server hosts the shared files and directories, while the clients access these resources over the network.

The client mounts the shared directory from the server, making it appear as a local directory. Once mounted, the client can read from and write to files on the server as if they were local files.

NFS uses Remote Procedure Calls (RPC) to communicate between the client and server. The server exports one or more directories that it wants to share, and the client mounts these directories to access them.

Benefits of NFS:
Centralized Storage: NFS allows for centralized storage of files and directories, making it easier to manage and backup data.

Cross-Platform Compatibility: NFS is supported by various operating systems, including Linux, Unix, and macOS, making it a versatile solution for file sharing in heterogeneous environments.

Scalability: NFS can scale to accommodate large numbers of clients accessing shared resources simultaneously, making it suitable for enterprise environments.

Getting Started:
To use NFS, you typically need access to a server that is configured to export directories for sharing and one or more client machines that mount these shared directories.
