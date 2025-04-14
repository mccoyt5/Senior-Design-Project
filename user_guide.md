# YAHOS User Guide
## Yet Another HOst Sentinel

YAHOS is a powerful system monitoring tool that provides real-time process monitoring, network analysis, and directory surveillance capabilities through kernel-level implementation.

## Table of Contents
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Core Features](#core-features)
- [User Interface Guide](#user-interface-guide)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)

## Installation

### System Requirements
- Windows Operating System (Windows 10 or later recommended)
- Administrator privileges
- Minimum 4GB RAM
- 500MB free disk space

### Installation Steps
1. Clone the latest YAHOS version from the official repository
2. Build the source and select "Run as Administrator" on the generated binary

## Getting Started

### First Launch
1. Launch YAHOS from the Start menu or desktop shortcut
2. The main window will display the process monitoring interface
3. Grant administrative privileges when prompted

### Basic Navigation
- The main window shows running processes with their Process IDs (PIDs)
- Use the "Refresh Table" button to update the process list
- Select processes to view detailed information
- Use "Choose File to Execute" to monitor new processes
- Click "Analyze" to perform detailed analysis of selected processes
- Click "Open Process Connections"  to see a detailed list of a specific process' active connections, or open ports

## Core Features

### Process Monitoring
- Real-time process tracking
- PID and process name display
- Service identification
- Memory usage analysis
- Process relationship mapping

### Network Monitoring
- Port activity tracking
- Packet capture and analysis
- Protocol identification
- Traffic pattern analysis

## User Interface Guide

### Main Window Elements
- Process List: Displays running processes with PIDs
- Notes Column: Shows additional process information
- Control Buttons:
  - Refresh Table: Updates the process list
  - Choose File to Execute: Opens file/directory selection
  - Analyze: Performs detailed analysis
  - Process Connections: Opens new window with a process' active connections

### Reading the Process List
- PID: Unique process identifier
- Process Name: Executable name with extension
- Connections: The number of connections, or ports, associated with the process
- Notes: Additional process information and flags

## Troubleshooting

### Common Issues
1. Access Denied
   - Ensure YAHOS is running with administrator privileges
   - Check Windows security settings
   - Verify user permissions

2. High Resource Usage
   - Reduce monitoring scope
   - Adjust sampling rates
   - Close unnecessary processes

3. Missing Processes
   - Click "Refresh Table"
   - Check process filtering settings
   - Verify system permissions

## FAQ

### General Questions

**Q: Does YAHOS impact system performance?**

A: YAHOS is designed to be lightweight, but intensive monitoring of multiple processes may affect system performance. Adjust monitoring parameters as needed.

**Q: Can YAHOS monitor encrypted traffic?**

A: YAHOS can monitor encrypted traffic patterns and metadata, but cannot decrypt encrypted communications.

**Q: How often should I refresh the process table?**

A: Manual refresh is recommended when a new process is started.

### Security Questions

**Q: Does YAHOS store sensitive data?**

A: YAHOS does not log any data, it only displays real-time system information.

**Q: Is YAHOS compatible with antivirus software?**

A: Yes, but you may need to add YAHOS to your antivirus exceptions list due to its kernel-level operations.

### Technical Questions

**Q: What kernel-level access does YAHOS require?**

A: YAHOS requires kernel-level access for process monitoring and network analysis. All access is strictly controlled and documented.

**Q: Can I monitor specific file types in directories?**

A: No. This feature has not been implemented into the latest version of YAHOS.

## Best Practices

1. Regular Updates
   - Keep YAHOS updated to the latest version
   - Review and update monitoring rules regularly

2. Resource Management
   - Monitor system resource usage
   - Adjust settings based on system capacity

3. Security
   - Use strong access controls

## Support

For additional support:
- Check the official documentation