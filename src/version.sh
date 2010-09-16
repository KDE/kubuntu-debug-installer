#!/bin/bash
dpkg-parsechangelog | sed -ne 's/^Version: \?\(.*\)/\1/p'
exit $?
