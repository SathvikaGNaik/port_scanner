# Port Scanner
Basic TCP/UDP Port Scanner in Linux C

# Overview
This project is a simple network port scanner developed in C as part of the Sasken Summer Internship 2025.
It scans TCP/UDP ports for a given IP address and logs the results with timestamps.

# Features
TCP/UDP port scanning
Multi-threaded implementation using pthreads
Command-line arguments for IP & port range
Logs scan results with timestamps (scan_results.txt)

# Usage
Compile
gcc scanner_combined.c -o scanner_combined -lpthread

Run
./scanner_combined -i <IP_ADDRESS> -s <START_PORT> -e <END_PORT> -t

Example
./scanner_combined -i 8.8.8.8 -s 20 -e 25 -t

Sample Output
[TCP] Port 53: OPEN [Wed Jul 16 06:11:49 2025]

# Skillset
Linux C Programming
Multithreading (pthreads)
Socket Programming
File Handling & Logging

# Team Contribution
This project was developed as part of Team 16 during the internship.
Team Members:
Sathvika G Naik
Anvitha C N
Athmashree U
Ranjitha T
Shrikanth
