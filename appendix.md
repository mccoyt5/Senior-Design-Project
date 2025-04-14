# Appendix

## Microsoft Documentation
- [PE Format](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format)
- [Kernel-Mode Driver Architecture Design Guide](https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/)
- [Tools for Software Tracing](https://learn.microsoft.com/en-us/windows-hardware/drivers/devtest/tools-for-software-tracing)
- [Event Tracing for Windows](https://learn.microsoft.com/en-us/windows-hardware/drivers/devtest/event-tracing-for-windows--etw-)
- [Adding ETW to Kernel-Mode Drivers](https://learn.microsoft.com/en-us/windows-hardware/drivers/devtest/adding-event-tracing-to-kernel-mode-drivers)
- [TraceLogging API](https://learn.microsoft.com/en-us/windows-hardware/drivers/devtest/tracelogging-api)
- [Peering Inside the PE: A Tour of the Win32 Portable Executable File Format](https://learn.microsoft.com/en-us/previous-versions/ms809762(v=msdn.10)?redirectedfrom=MSDN)

## Docker
- [Network Drivers](https://docs.docker.com/engine/network/drivers/)
- [Volumes](https://docs.docker.com/engine/storage/volumes/)
- [Isolate containers with a user namespace](https://docs.docker.com/engine/security/userns-remap/)
- [Configure logging drivers](https://docs.docker.com/engine/logging/configure/)

## Articles
- [Event Tracing for Windows (ETW): Your Friendly Neighborhood IPC Mechanism](https://www.preludesecurity.com/blog/event-tracing-for-windows-etw-your-friendly-neighborhood-ipc-mechanism)
- [ETW: Event Tracing for Windows 101](https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/etw-event-tracing-for-windows-101)
- [Parsing PE File Headers with C++](https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/pe-file-header-parser-in-c++)

## Fall Semester Hours Worked
| Work Category | Guillermo Rached | Thomas McCoy
|-|-|-|
| Senior Design Course Work | 12 | 12 |
| Research & Independent Learning | 18 | 18 |
| Document Revision | 3 | 3 |
| Meetings with Team and Advisor | 12 | 12 |

*Guillermo Total Hours: 45*

*Thomas Total Hours: 45*

## Spring Semester Hours Worked

| Work Category | Guillermo Rached | Thomas McCoy
|-|-|-|
| Senior Design Course Work | 12 | 12 |
| Research & Independent Learning | 10 | 15 |
| Component Design | 3 | 3 |
| Component Implementation | 15 | 20 |
| Component Testing | 5 | 4 |
| Code Revision | 3 | 2 |
| Document Revision | 2 | 2 |
| Meetings with Team and Advisor | 10 | 10 |

### Guillermo Hours Justification
*Guillermo Total Hours: 60*

I thoroughly immersed myself in Microsoft's extensive documentation covering the Windows Process API, security models, and network stack integration. This involved studying multiple monitoring approaches (WMI, Process Status API, ETW) to determine optimal performance characteristics while maintaining system stability. I spent considerable time analyzing the PE file structure to understand implementation patterns and potential unique features for our tool. The remaining hours were dedicated to learning and researching Qt framework implementation strategies, particularly focusing on architecture patterns suitable for real-time monitoring applications, threading models for responsive UIs, and optimizing performance for intensive monitoring scenarios. Each research segment directly informed the subsequent design decisions, component implementation strategies, testing approaches, and ultimately shaped both the code review criteria and comprehensive documentation that would accompany the finished product. All mentioned activities can be observed through this repository's commit messages and progress on the development of the product.

#### Total hours: 105

### Thomas Hours Justification
*Thomas Total Hours: 68*

My hours in the beginning of the project were spent largely on reading the Qt documentation. This was very important as Qt was used to create the user interface of the project. After that, I spent a lot of time reading the Microsoft API documentation. This was necessary to learn how to display a table containing all of the running processes on the machine. Once that was implemented, I had to continue reading the Microsoft API documentation as well as other articles on PE file structure and how to extract information from them. This took a considerable amount of time as I had no prior experience working with them. This also led to bugs when implementing these features, that I had to spend time on to resolve. This work, in addition to the documents in the repository, are how I was able to come to this total.

#### Total hours: 113
