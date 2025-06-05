#!/bin/bash

set -x

# store
# find xa2plus -iname "*.png" -execdir sh -c ' for f; do convert -verbose -geometry 1080x "$f" "../store/${f##.*/}"; done ' find-sh {} +
# openrepos
# find xa2plus -iname "*.png" -execdir sh -c ' for f; do convert -verbose -geometry 540x "$f" "../openrepos/${f##.*/}"; done ' find-sh {} +
# homepage
find xa2plus -iname "*.png" -execdir sh -c ' for f; do convert -verbose -geometry 360x "$f" "../homepage/small_${f##.*/}"; done ' find-sh {} +

