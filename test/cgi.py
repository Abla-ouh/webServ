#!/usr/bin/env python3

#  generate an HTTP header that prints the argumnets passed to the script

import sys

def main():
    # Get all command-line arguments except the script name itself
    print("Content-Type: text/plain\r\n")
    print("\r\n")
    arguments = sys.argv[1:]
    
    if arguments:
        print("Arguments passed:")
        for arg in arguments:
            print(arg)
    else:
        print("No arguments passed.")

if __name__ == "__main__":
    main()