#!/bin/bash

# Login to registry for repo
docker login -u chasschool -p changemepassword

# Pull down latest image
docker pull chasschool/drmayday:latest

# First time deploy
docker stack update --image chasschool/drmayday:latest --with-registry-auth drmayday_api

# Set exit code depending on status
if [ $? -eq 0 ]
then
	echo "Successfully updated DrMayday!"
		exit 0
	else
		echo "Update failed" >&2
		exit 1
fi
