#!/bin/sh
# QDOS has a diffirent file naming scheme
# We have to name the source files appropriately for
# compilation on QDOS

rename 's/\.h$/_h/g' *
rename 's/\.c$/_c/g' *
