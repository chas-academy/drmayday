#!/bin/bash

# Login to registry for repo
docker login -u chasschool -p changemepassword

# Pull down latest image
docker pull chasschool/drmayday:latest

# First time deploy
docker stack deploy -c docker-stack.yml --with-registry-auth drmayday

# Set exit code depending on status
if [ $? -eq 0 ]
then
	echo "Successfully deployed DrMayday!"
		exit 0
	else
		echo "Deploy failed" >&2
	exit 1
fi
