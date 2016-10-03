#!/bin/sh
rm -rf _site
git clone -b gh-pages `git config remote.origin.url` _site
bundle exec jekyll b
cd _site
git add -A
git commit -am 'Yeah. Built from subdir'
git push
cd ..
