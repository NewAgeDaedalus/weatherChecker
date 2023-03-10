# weatherChecker

Command line utility for checking the weather using the openweathermap.org API.

## Compilation and installation 
Get your API key from https://openweathermap.org/

Insert your API key in the main.c by editing it manually or 
run the command below

```bash
sed 's/apiKey = ""/apiKey = "[YOUR API KEY]"/' main.c -i
```

To compile and install run the command

```bash
make && sudo make install
```

## Usage

To get the current weather of a place use

```bash
weatherChecker "New York"
```

To get the weather report for the next few hours use

```bash
weatherChecker "Berlin" -h 6
```
