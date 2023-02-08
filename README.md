# weatherChecker

Command line utility for checking the weather using the openweathermap.org API.

## Compilation and installation 
Get your API key from https://openweathermap.org/

Insert your API key in the main.c by editing it manually or 
run the command below
'''
sed 's/apiKey = ""/apiKey = "[YOUR API KEY]"/' main.c -i
'''

To compile and install run the command
'''
make && make install
'''

## Usage

To get the current weather of a place use

'''
weather "New York"
'''

To get the weather report for the next few hours use

'''
weather "Berlin" -h 6
'''
