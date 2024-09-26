# Task List

## Overhead [Guillermo & Thomas]
1. Research Windows API calls
2. Research Windows Native API
3. Research Docker environment / Sandbox isolation
4. Research Network protocols / common packets for software

## YAHOS
1. Research how to attach to an existing process similarly to PROC MON [Guillermo]
2. Develop Network Module [Thomas]
    1. pcap file parsing or binary stream parsing
3. Develop Host Module [Guillermo]
    1. Research how to track process events through windows API
4. Develop a standard for data output [Guillermo & Thomas]
    1. How to report network data? What information to show? Host changes? In what form to store?
5. Develop report module [Guillermo & Thomas]
    1. Create HTML report pipeline based on output standards

## Docker
1. Create/Build Dockerfile for windows image [Guillermo]
    1. Linux image
2. Create docker-compose.yml file [Thomas]
    1. Specify network modes and internal networks.
3. Create mounted volume for data gathering [Guillermo]
    1. Host/Network data
    2. Research if this can backfire on host if loading suspicious software
4. Test images and containers [Thomas]
    1. Can we run PE files in windows container?
    2. Does traffic get caught by InetSim?

## QT - UI
1. Research QT workflow for C/C++ programming [Guillermo & Thomas]
2. Create UI components for each module [Guillermo & Thomas]
3. Compile components [Thomas]
