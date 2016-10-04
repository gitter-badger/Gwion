#!/bin/sh

# deploy site on gh-pages
function deploy()
{
	git clone -b gh-pages `git config remote.origin.url` _site
	mv README.md  README.old
	echo 'built by site/mk_size.sh' > README.md
	echo '_site' > .gitignore
	bundle exec jekyll s --config _config.yml,_config_deploy.yml
	cd _site
	git add -A
	git commit -am 'Yeah. Built from subdir'
	git push
	cd ..
	rm -rf _site
	mv README.old README.md
	rm Gemfile.lock
	git add -A
	git commit -am 'Yeah. Built from subdir'
	git push > /dev/null
}

#run the size locally. access with localhost:4000
function run()
{
	bundle exec jekyll s
	rm -rf _site
}

# remove _site if interupted or ...
trap 'rm -rf _site; exit' INT
trap 'rm -rf _site; exit' QUIT
trap 'rm -rf _site; exit' TERM
trap 'rm -rf _site; exit' EXIT

# parse arguments
if("$1" == "run")
then
	run 1
elif("$1" == "deploy")
then
	./deploy 1
else
	echo "$0 needs 'run' or 'deploy' as argument"
fi
