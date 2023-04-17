# TCP Congestion Control Benchmarking Tool

This is a TCP congestion control benchmarking tool that measures the performance of the `cubic` and `reno` congestion control algorithms. It includes a sender and a receiver that communicate over TCP, with the sender sending a large amount of data to the receiver using a specific congestion control algorithm.

## Requirements

* Linux operating system
* GCC compiler

## Installation

1. Clone the repository: `git clone https://github.com/username/repo.git`
2. Change directory: `cd repo`
3. Build the executable files: `make all`

## Usage

1. Start the receiver by running the `Receiver` executable: `./Receiver`
2. Start the sender by running the `Sender` executable: `./Sender`
3. Wait for the sender to finish sending data.
4. The program will output the results of the benchmarking test.

## Authors

- Maya Rom
- Yogev Ofir
