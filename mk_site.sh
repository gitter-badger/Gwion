#!/bin/sh
git clone -b gh-pages `git config remote.origin.url` _site
mv README.md  README.old
echo 'built by site/mk_size.sh' > README.md
echo '_site' > .gitignore
bundle exec jekyll b
cd _site
git add -A
git commit -am 'Yeah. Built from subdir'
git push
cd ..
rm -rf _site
mv README.old README.md
git add -A
git commit -am 'Yeah. Built from subdir'
git push
