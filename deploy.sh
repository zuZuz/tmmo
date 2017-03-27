#!/bin/bash

git config --global user.name "Aidar"
git config --global user.email "zreacta@gmail.com"
git tag -a v0.${TRAVIS_BUILD_NUMBER} -m "Autobuild v0.$TRAVIS_BUILD_NUMBER from Travis."
git push -q https://user:${GITHUB_TOKEN}@github.com/zuZuz/test --tags

tar czf client-v0.${TRAVIS_BUILD_NUMBER}.tar.gz bin/client/
tar czf server-v0.${TRAVIS_BUILD_NUMBER}.tar.gz bin/server/