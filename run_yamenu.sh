#!/bin/bash

# this script executes yamenu with all possible commands
# this script depends on bash!

# programs=$(compgen -c | sort | tr '\n' ';')
programs=$(ls /usr/share/applications | awk -F '.desktop' '{ print $1 }' | sort | tr '\n' ';')
yamenu -p"$programs"
