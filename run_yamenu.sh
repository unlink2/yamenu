#!/bin/bash

# this script executes yamenu with all possible commands
# this script depends on bash!

programs=$(compgen -c | sort | tr '\n' ';')
yamenu -p"$programs"
