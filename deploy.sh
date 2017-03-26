#!/bin/bash

# source in http://szagurskii.com

branch="master"

if [ "$TRAVIS_BRANCH" = "$BRANCH" ]; then
	if [ "$TRAVIS_PULL_REQUEST" = false ]; then
		if [ -z "$TRAVIS_TAG" ]; then
			git tag -a v0.${TRAVIS_BUILD_NUMBER} -m "Autobuild v0.$TRAVIS_BUILD_NUMBER."
			git push origin --tags
			git fetch origin
		fi
	fi
fi